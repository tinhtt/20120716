#pragma once
#include "../VNetWork/VUser.h"
#include "VChatFwd.h"

namespace vchat
{
	class VChatUser : public VUser, public boost::enable_shared_from_this<VChatUser>
	{
	public:
		VChatUser(void);
		virtual ~VChatUser(void);

		//static void SendData(VSessionPtr pSession, const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen);

	private:		
		VInt32	m_nServerID;
		VInt32	m_nPlayerID;
		VString	m_strUserName;

	public:
		const VInt32 GetServerID() const {
			return m_nServerID;
		}

		const VInt32& GetPlayerID() const { 
			return m_nPlayerID; 
		}

		const VString &GetUserName() const {
			return m_strUserName;
		}

		void SendPacket(const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen);

		virtual void CloseSession();
	};
};
