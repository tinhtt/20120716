#pragma once
#include "VCommonDefines.h"
#include "VUser.h"
#include "VChatPacketImpl.h"
#include "VChatRoom.h"

namespace vchat
{
	class VChatUser : public VUser, public boost::enable_shared_from_this<VChatUser>
	{
	public:
		VChatUser(void);
		virtual ~VChatUser(void);

	public:
		static void SendData(SessionPtr pSession, const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen);

	private:		
		VChatRoomRef m_Room;
		VString	m_strUserName;
		VInt32	m_nPlayerID;
		VInt32	m_nServerID;

		// IPSec
		VUInt32 m_CrcTable[256];
		VUInt32 m_nSeq;
		VUInt32 m_nKey;
		VBool	m_bGM;

		mutable VMutex m_Mutex;

	public:
		const VBool IsGM() const
		{
			VReadLock lock(m_Mutex);
			return m_bGM;
		}

		void SetGM(const VBool	&bGM)
		{
			m_bGM = bGM;
		}

		const VInt32 GetServerID() const {
			VReadLock lock(m_Mutex);
			return m_nServerID;
		}

		void SetServerID(const VInt32 &nServerID) {
			VWriteLock lock(m_Mutex);
			m_nServerID = nServerID;
		}

		const VInt32& GetPlayerID() const { 
			VReadLock lock(m_Mutex);
			return m_nPlayerID; 
		}

		void SetPlayerID(const VInt32& val) { 
			VWriteLock lock(m_Mutex);
			m_nPlayerID = val;
		}

		VChatRoomRef GetRoom() const { 
			VReadLock lock(m_Mutex);
			return m_Room; 
		}

		VInt32 GetRoomId() {
			VReadLock lock(m_Mutex);
			return m_Room ? m_Room->GetId() : -1;
		}

		void SetRoom(const VChatRoomRef &room) { 
			VWriteLock lock(m_Mutex);
			m_Room = room;
		}

		void ResetRoom() { 
			VWriteLock lock(m_Mutex);
			m_Room.reset(); 
		}
		
		const VString &GetUserName() const {
			VReadLock lock(m_Mutex);
			return m_strUserName;
		}

		void SetUserName(const VString &strName) {
			VWriteLock lock(m_Mutex);
			m_strUserName = strName;
		}

		const VInt32 GetMatchMakingPoint() const;

		void JoinRoom(const VInt32& nLobbyRoomId, const VInt32& nRoomMode = 0);
		void OnChatMsg(const Packet::ChatRoom* msg);
		void ReceiveChatMsg(VChatUser* sender, const Packet::ChatRoom* msg);
		void SendPacket(const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen);
		void Close();
		void Reset();

		virtual void CloseSession();
	};
};
