#include "VChat.h"
#include "VCommonDefines.h"
#include "crc32.h"
#include "VSocialDB.h"
#include "VChatUser.h"
#include "VUserManager.h"
#include "VDatabaseManager.h"
#include "VChatRoomManager.h"

namespace vchat
{
	VChatUser::VChatUser(void) : m_nServerID(0), m_nKey(0), m_nSeq(0), m_bGM(VFalse) {}

	VChatUser::~VChatUser(void) {}

	void VChatUser::SendData( SessionPtr pSession, const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen )
	{
		ChatHeader header;
		header.type = nPacketType;
		header.l = nLen;
		VUser::SendData(pSession, ConstBuffer(sizeof(header), (const char*)&header), ConstBuffer(nLen,(const char*)pPacket));
	}

	void VChatUser::Reset()
	{	
		VWriteLock lock(m_Mutex);
		m_Room.reset();
	}
	
	void VChatUser::JoinRoom(const VInt32& nLobbyRoomId, const VInt32& nRoomMode)
	{
		VInt32 nChatRoomId = nLobbyRoomId + m_nServerID * (VInt32)vchat::SequenceRoom;
		if( GetRoomId() != nChatRoomId)
		{
			if( m_Room )
				m_Room->RemoveUsr(this->shared_from_this());
			VChatRoomManager::Instance().UserJoinRoom(this->shared_from_this(), nChatRoomId, nRoomMode);
		}
	}

	/*void VChatUser::ReceiveChatMsg(VChatUser* sender,const Packet::ChatRoom* msg)
	{
	}*/

	void VChatUser::SendPacket(const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen)
	{
		VReadLock lock(m_Mutex);
		ChatHeader header;
		header.type = nPacketType;
		header.l = nLen;
		VUser::SendData(ConstBuffer(sizeof(header), (const char*)&header), ConstBuffer(nLen,(const char*)pPacket));
	}

	void VChatUser::OnChatMsg(const Packet::ChatRoom* msg)
	{
	}

	void VChatUser::Close()
	{
		VWriteLock lock(m_Mutex);	
		m_strUserName.clear();
		m_nSeq = 0;
		m_nKey = 0;
	}

	void VChatUser::CloseSession()
	{	
		SetState(VUser::LOGOUT);
		VDatabaseManager::Instance().ProcessLogOut(m_nPlayerID);
		VUserManager::Instance().RemoveUser(this);	
	}

	const VInt32 VChatUser::GetMatchMakingPoint() const
	{
		using namespace VSocialDB::vchat;
		VReadLock lock(m_Mutex);
		UserDBRef pRole = UserTable::Instance().FindRecordEx(m_nPlayerID);
		if( pRole )
		{
			UserDB::RecordType record = pRole->GetRecord();
			return record.Attr.nLevel;
		}
		return 0;
	}

};
