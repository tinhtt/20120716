#include "VChat.h"
#include "VChatRoom.h"
#include "VChatUser.h"
#include "VEnum.h"
#include "VLogManager.h"

namespace vchat
{
	VBool VChatRoom::IsMatchMakingMode( const VInt32& nMode )
	{
		switch (nMode)
		{
		case GAME_MODE_PVP_DEATHMATCH_MM:
		case GAME_MODE_PVP_BASE_PROTECTION_MM:
			return VTrue;
		default:
			return VFalse;
		}
	}

	VChatRoom::VChatRoom(void)
		: m_nId(0)
		, m_nServerId(-1)
		, m_nGameMode(-1)		
	{
	}

	VChatRoom::~VChatRoom(void)
	{
	}

	VChatUserRef VChatRoom::FindUserByID(const VInt32& nUserID)
	{
		VReadLock lock(m_Mutex);
		UserContainerIt itfind = std::find_if(m_Users.begin(), m_Users.end(), boost::bind(&VChatUser::GetPlayerID, _1) == nUserID);
		return itfind == m_Users.end() ? VChatUserRef() :*itfind;
	}

	void VChatRoom::AddUsr(const VChatUserRef &usr)
	{
		VWriteLock lock(m_Mutex);
		m_nRoomStatus = ROOM_NORMAL;
		m_Users.push_back(usr);		
		m_nServerId = usr->GetServerID();
		usr->SetRoom(this->shared_from_this());
	}

	void VChatRoom::RemoveUsr(const VChatUserRef &usr)
	{
		VWriteLock lock(m_Mutex);
		UserContainerIt it = std::find(m_Users.begin(), m_Users.end(), usr);
		if( it != m_Users.end() )
		{
			m_Users.erase(it);
			if (m_Users.empty())
			{
				m_nServerId = -1;
				m_nGameMode = -1;
			}			
			usr->ResetRoom();
			if (m_nRoomStatus == ROOM_ENQUEUE_MM)
			{
				std::for_each(m_Users.begin()
								, m_Users.end()
								, boost::bind(&VChatUser::SendPacket, _1, vchat::Packet::s2c_stop_mm_timer, (const void*)0, 0));
			}

			m_nRoomStatus = ROOM_NORMAL;
		}			
	}

	void VChatRoom::SendPacket(const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen)
	{		
		ChatHeader header;
		header.type = nPacketType;
		header.l = nLen;
		VSimpleBufferPtr buffer = boost::make_shared<VSimpleBuffer>(&header, sizeof(header), pPacket, nLen);
		if( !buffer->isValid() )
			return;
		//std::for_each(m_Users.begin(), m_Users.end(), boost::bind(&VChatUser::SendPacket, _1, nPacketType, pPacket, nLen));
		VReadLock lock(m_Mutex);
		std::for_each(m_Users.begin(), m_Users.end(), boost::bind(&VChatUser::SendBuffer, _1, buffer));
	}

	VBool VChatRoom::GetCriteria(MMCriteria &criteria)
	{
		VReadLock lock(m_Mutex);
		if( m_Users.empty() )
			return VFalse;
		criteria.nCriteria[0] = m_nServerId;
		criteria.nCriteria[1] = m_nGameMode;
		criteria.nCriteria[2] = (VInt32)m_Users.size();
		criteria.nCriteria[3] = 0;
		for (UserContainerIt it = m_Users.begin(), e = m_Users.end(); it != e; ++it)
			criteria.nCriteria[3] += (*it)->GetMatchMakingPoint();
		return VTrue;
	}

	VBool VChatRoom::StartMM()
	{
		VWriteLock lock(m_Mutex);
		if (m_nRoomStatus != ROOM_ENQUEUE_MM)
		{
			m_nRoomStatus = ROOM_ENQUEUE_MM;
			m_MMStartTime = VCurrentTime();
			return VTrue;
		}
		return VFalse;
	}

	VBool VChatRoom::StopMM()
	{
		VWriteLock lock(m_Mutex);
		if (m_nRoomStatus == ROOM_ENQUEUE_MM)
		{
			m_nRoomStatus = ROOM_NORMAL;
			return VTrue;
		}
		return VFalse;
	}

	VUInt32 VChatRoom::GetMMTime()
	{
		boost::posix_time::time_duration MMDuration = VCurrentTime() - m_MMStartTime;
		return MMDuration.total_seconds();
	}

	VUInt32 VChatRoom::GetMMPoint()
	{
		VReadLock lock(m_Mutex);
		if( m_Users.empty() )
			return 0;

		VUInt32 nTotal = 0;
		for (UserContainerIt it = m_Users.begin(), e = m_Users.end(); it != e; ++it)
			nTotal += (*it)->GetMatchMakingPoint();
		return nTotal;
	}

	VUInt32 VChatRoom::GetUserCount()
	{
		VReadLock lock(m_Mutex);
		return m_Users.size();
	}

	void VChatRoom::WriteLogMatchMaking(const VChatRoomRef& pRoom1, const VChatRoomRef& pRoom2, const VUInt8 nResult)
	{
		if (!pRoom1)
			return;

		if (pRoom1->GetUserCount() <= 0 || pRoom1->GetUserCount() > 5)
			return;

		/*
		ServerID	: int
		RoomID1		: int
		RoomID2		: int
		RoomName1	: varchar
		RoomName2	: varchar
		MatchTime1	: int
		MatchTime2	: int
		SumLevel1	: int
		SumLevel2	: int
		MatchType	: varchar
		Result		: varchar
		*/

		static const char *MatchType[] = {"1vs1", "2vs2", "3vs3", "4vs4", "5vs5"};
		static const char *Result[] = {"Success", "Canceled"};

		if (pRoom2)
			VLogManager::Instance().WriteLog(VLogManager::LOG_MATCH_MAKING
											, "%d\t%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%s\t%s"
											, pRoom1->GetServerId()
											, pRoom1->GetId()
											, pRoom2->GetId()
											, "NA1"
											, "NA2"
											, pRoom1->GetMMTime()
											, pRoom2->GetMMTime()
											, pRoom1->GetMMPoint()
											, pRoom2->GetMMPoint()
											, MatchType[pRoom1->GetUserCount() - 1]
											, Result[nResult]
											);
		else
			VLogManager::Instance().WriteLog(VLogManager::LOG_MATCH_MAKING
											, "%d\t%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%s\t%s"
											, pRoom1->GetServerId()
											, pRoom1->GetId()
											, 0
											, "NA1"
											, "NA2"
											, pRoom1->GetMMTime()
											, 0
											, pRoom1->GetMMPoint()
											, 0
											, MatchType[pRoom1->GetUserCount() - 1]
											, Result[nResult]
											);
	}
};