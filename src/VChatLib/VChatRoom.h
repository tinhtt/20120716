#pragma once
#include "VCommonDefines.h"
#include "VChatUserFwd.h"
#include "VChat.h"
namespace vchat
{	
	class VChatRoom : public boost::enable_shared_from_this<VChatRoom> 
	{
	public:
		enum
		{
			ROOM_NORMAL,
			ROOM_ENQUEUE_MM,
			ROOM_FOUND_MM
		};
	public:
		typedef VList<VChatUserRef>		UserContainer;
		typedef UserContainer::iterator	UserContainerIt;

	public:
		static void WriteLogMatchMaking(const VChatRoomRef& pRoom1, const VChatRoomRef& pRoom2, const VUInt8 nResult);

		VChatRoom();
		~VChatRoom();

	public:
		void SetId(const VInt32 &Id)	{
			VWriteLock lock(m_Mutex);
			m_nId = Id;
		}

		const VInt32& GetId() const	{
			VReadLock lock(m_Mutex);
			return m_nId;
		}

		void SetGameMode(const VInt8 &mode)	{
			VWriteLock lock(m_Mutex);
			m_nGameMode = mode;
		}

		const VInt8& GetGameMode() const	{
			VReadLock lock(m_Mutex);
			return m_nGameMode;
		}

		void SetServerId(const VInt32 &id)	{
			VWriteLock lock(m_Mutex);
			m_nServerId = id;
		}

		const VInt32& GetServerId() const	{
			VReadLock lock(m_Mutex);
			return m_nServerId;
		}

		void SetStatus(const VInt32& value)	{
			VWriteLock lock(m_Mutex);
			m_nRoomStatus = value;
			if (m_nRoomStatus == ROOM_FOUND_MM)
			{
				lock.unlock();
				SendPacket(vchat::Packet::s2c_mm_room_found, 0, 0);
			}
		}

		const VInt32& GetStatus() const	{
			VReadLock lock(m_Mutex);
			return m_nRoomStatus;
		}

		template<typename Handler, typename Msg>
		void ForEachUser(Handler handler, const Msg *msg)
		{
			VReadLock lock(m_Mutex);
			for (UserContainerIt it = m_Users.begin(), e = m_Users.end(); it != e; ++it)
				handler(*it, msg);
		}

		UserContainer GetUsers(VInt32 nFrom,VInt32 nTo) 
		{ 
			VWriteLock lock(m_Mutex);
			UserContainer arrPlayer;
			arrPlayer.clear();
			if( nTo < 0 )
				nTo = (VInt32)m_Users.size();
			if( nFrom < nTo )
			{								
				VInt32 i = 0;
				for (UserContainerIt it = m_Users.begin(), e = m_Users.end();it != e ; it++)
				{
					if( i >= nTo )
						break;
					else if( i >= nFrom )
						arrPlayer.push_back(*it);
					++i;
				}
			}
			return arrPlayer;
		}

		VBool StartMM();
		VBool StopMM();
		VUInt32 GetMMTime();
		VUInt32 GetMMPoint();
		VUInt32	GetUserCount();

		VChatUserRef FindUserByID(const VInt32& nUserID);
		void AddUsr(const VChatUserRef &usr);
		void RemoveUsr(const VChatUserRef &usr);
		void SendPacket(const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen);		
		VBool GetCriteria(MMCriteria &criteria);

	private:
		VInt32	m_nId;
		VInt32	m_nServerId;	
		VInt8	m_nGameMode;
		VInt32	m_nRoomStatus;
		VTime	m_MMStartTime;

		UserContainer	m_Users;

		mutable SharedMutex	m_Mutex;

	public:
		static VBool IsMatchMakingMode(const VInt32& nMode);
	};	
};
