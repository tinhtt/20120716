#pragma once
#include "VChatUserFwd.h"

namespace vchat
{
	class VChatProcessPacketManager : public VSingleton<VChatProcessPacketManager>
	{	 	
	private:
		void (VChatProcessPacketManager::*ProcessFunction[Packet::c2s_gs_count])(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize);

	public:
		VChatProcessPacketManager(void);
		~VChatProcessPacketManager(void);
		void ProcessPacket(VChatUserPtr pPlayer, const ChatHeader *header, const ConstBuffer &body);
	
	private:
		void ProcessChat(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize);
		void ProcessJoinRoom(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize);
		void ProcessBanNick(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize);
		void ProcessNotifyGlobal(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize);
		void RoomEnterMMQueue(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize);
		void RoomLeaveMMQueue(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize);
		void ProcessKickUser(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize);
		void RequestZMEFriends(VChatUserPtr pPlayer, const VChar *pData, const VUInt32 &nLen);
		void CodeZME(VChatUserPtr pPlayer, const VChar *pData, const VUInt32 &nLen);
		void RequestZCBillingURL(VChatUserPtr pPlayer, const VChar *pData, const VUInt32 &nLen);

	public:
		void SendBillingURL(const VInt32 &nPlayerID, const VString &strBillingURL);
	};
};
