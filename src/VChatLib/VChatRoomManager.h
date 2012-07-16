#pragma once
#include "VCommonDefines.h"
#include "VChatUser.h"
#include "VChatRoom.h"

namespace vchat
{
	class VChatRoomManager : public VSingleton<VChatRoomManager>
	{
		typedef VCommon::ObjectRefAllocator<VChatRoom>		RoomAllocator;
		typedef VCommon::DenseHashMap<VInt32, VChatRoomRef>	VChatRoomContainer;
		typedef VChatRoomContainer::iterator				VChatRoomIt;
		typedef VPair<VInt32, VChatRoomRef>					VChatRoomValue;
		typedef	VSharedPtr<VChatUser>	VChatUserRef;

	public:
		VChatRoomManager(void)	{	m_Rooms.set_empty_key(-1);	}
		~VChatRoomManager(void)	{}

	public:

		void UserJoinRoom(const VChatUserRef &usr, const VInt32& nChatRoomId, const VInt32& nRoomMode)
		{
			if( nChatRoomId != -1)
			{
				ScopedLock lock(m_Mutex);
				VChatRoomIt it = m_Rooms.find(nChatRoomId);
				if( it != m_Rooms.end() )
				{
					it->second->AddUsr(usr);
					if (it->second->GetGameMode() == -1 || it->second->GetServerId() == -1)
					{
						it->second->SetGameMode(nRoomMode);
						it->second->SetServerId(usr->GetServerID());
					}
				}
				else
				{
					VChatRoomRef room = RoomAllocator::Create();
					room->AddUsr(usr);
					room->SetId(nChatRoomId);
					room->SetGameMode(nRoomMode);
					m_Rooms.insert(VChatRoomValue(nChatRoomId, room));
				}
			}
		}

		void RemoveRoom(VInt32 rid)
		{
			ScopedLock lock(m_Mutex);
			m_Rooms.erase(rid);		
		}

		template<typename Container>
		void GetMMRooms(Container &list)
		{
			/*VectorInt32 listCriteria;
			listCriteria.resize(4, 0);*/
			MMCriteria criteria;

			ScopedLock lock(m_Mutex);
			for (VChatRoomIt it = m_Rooms.begin(); it != m_Rooms.end(); it++)
				if (VChatRoom::IsMatchMakingMode(it->second->GetGameMode()) && it->second->GetStatus() == VChatRoom::ROOM_ENQUEUE_MM)
				{
					if( it->second->GetCriteria(criteria) )
						list.push_back(std::make_pair(criteria, it->second));
				}
		}

		VChatRoomRef GetRoomByID(const VInt32& nRoomID){ 
			ScopedLock lock(m_Mutex);
			VChatRoomIt it = m_Rooms.find(nRoomID); 
			if (it != m_Rooms.end())
				return it->second;
			else
			{
				VChatRoomRef nullRoom;
				nullRoom.reset();
				return nullRoom;
			}
		}

	private:
		VChatRoomContainer  m_Rooms;
		Mutex m_Mutex;
	};
};
