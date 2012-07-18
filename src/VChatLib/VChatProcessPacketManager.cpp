#include "VChat.h"
#include "VChatProcessPacketManager.h"
#include "VChatUser.h"
#include "VLogManager.h"
#include "VUserManager.h"
#include "VChatUtility.h"
#include "VDatabaseManager.h"
#include "VLobby2SocialProcessPacket.h"
#include "VZMEManager.h"
#include "md5.h"
#include "boost/format.hpp"
#include "VSocialDB.h"

namespace vchat
{
	static void WriteLogChat(VChatUserPtr usr, const char *pszContent, const VInt32 &nType = 1)
	{
		/*
		LogTime	Thời gian login	yyyy-mm-dd hh:mm:ss
		ServerID	ID của server	int
		ActionType	Kênh chat: mật/room/bang hội/ thế giới…	varchar	
		RoleName	Tên Role	nvarchar
		RoleID	ID Role	int
		ActionIP	IP  v4	varchar	
		Contents	Nội dung chat	nvarchar
		*/
		const char *ActionType[] = { "private", "room", "group", "world"};
		VLogManager::Instance().WriteLog(VLogManager::LOG_CHAT, 
			"%d\t%s\t%s\t%d\t%s\t%s",
			usr->GetServerID(),
			ActionType[nType],
			usr->GetUserName().c_str(),
			usr->GetPlayerID(),
			VUser::GetSessionAddress(usr->GetSession()).c_str(),
			pszContent ? pszContent : " "
			);

	}

	VChatProcessPacketManager::VChatProcessPacketManager(void)
	{
		ProcessFunction[Packet::c2s_gs_join_room]				=	&VChatProcessPacketManager::ProcessJoinRoom;
		ProcessFunction[Packet::c2s_gs_chat_message]			=	&VChatProcessPacketManager::ProcessChat;
		ProcessFunction[Packet::c2s_gs_ban_nick]				=	&VChatProcessPacketManager::ProcessBanNick;
		ProcessFunction[Packet::c2s_gs_tb_global]				=	&VChatProcessPacketManager::ProcessNotifyGlobal;
		ProcessFunction[Packet::c2s_gs_enter_queue]				=	&VChatProcessPacketManager::RoomEnterMMQueue;
		ProcessFunction[Packet::c2s_gs_leave_queue]				=	&VChatProcessPacketManager::RoomLeaveMMQueue;
		ProcessFunction[Packet::c2s_gs_kick_user]				=	&VChatProcessPacketManager::ProcessKickUser;
		ProcessFunction[Packet::c2s_request_zme_friends]		=	&VChatProcessPacketManager::RequestZMEFriends;
		ProcessFunction[Packet::c2s_send_code_zme]				=	&VChatProcessPacketManager::CodeZME;
		ProcessFunction[Packet::c2s_request_zc_billing_url]		=	&VChatProcessPacketManager::RequestZCBillingURL;
	}

	VChatProcessPacketManager::~VChatProcessPacketManager(void)
	{
	}

	void VChatProcessPacketManager::ProcessChat(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize)
	{
		if( usr && packet && nSize > 1 && packet[nSize - 1] == 0)
		{
			
#if 1
			WriteLogChat(usr, packet);
			vchat::VChatRoomRef pRoom = usr->GetRoom();
			if( pRoom )
			{
				if( (pRoom->GetId() % SequenceRoom) == 0 )// o lobby thi co the chat het toan bo server
				{
					VBuffer bufferChat = CreateBufferChat(vchat::CHAT_SERVER,usr->GetPlayerID());
					bufferChat.WriteBuffer(usr->GetUserName().c_str(),usr->GetUserName().size());
					bufferChat.WriteObject(':');
					bufferChat.WriteBuffer(packet, nSize);
					//VUserManager::Instance().SendServerPacket(vchat::Packet::s2c_chat_message,bufferChat.GetBuffer(),bufferChat.GetSize(), usr->GetServerID());
					pRoom->SendPacket(vchat::Packet::s2c_chat_message,bufferChat.GetBuffer(),bufferChat.GetSize());
				}
				else
				{
					VBuffer bufferChat = CreateBufferChat(vchat::kChatNormal,usr->GetPlayerID());
					bufferChat.WriteBuffer(usr->GetUserName().c_str(),usr->GetUserName().size());
					bufferChat.WriteObject(':');
					bufferChat.WriteBuffer(packet, nSize);
					pRoom->SendPacket(vchat::Packet::s2c_chat_message,bufferChat.GetBuffer(),bufferChat.GetSize());
				}
			}
#else
			vchat::VChatRoomRef pRoom = usr->GetRoom();
			if (pRoom)
			{
				WriteLogChat(usr, packet);
				pRoom->SendPacket(vchat::Packet::s2c_chat_message,bufferChat.GetBuffer(),bufferChat.GetSize());
			}		
#endif
		}
	}

	void VChatProcessPacketManager::ProcessJoinRoom(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize)
	{
		if( usr )
		{
			const Packet::JOIN_ROOM_PACKET *joinroom = (const Packet::JOIN_ROOM_PACKET *)(packet);
			usr->JoinRoom(joinroom->nRoomID, joinroom->nRoomMode);
		}
	}

	void VChatProcessPacketManager::ProcessBanNick(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize)
	{
		if( usr && usr->IsGM() && packet && packet[0])
		{		
			VInt32 Id = VDatabaseManager::Instance().GetRoleId(packet);
			if( Id >= 0 )
			{
				VChatUserRef usr_ban = VUserManager::Instance().GetUser(Id);
				if( usr_ban )
				{
					usr_ban->SetState(VUser::KICK);
					VDatabaseManager::Instance().AddUserBanId(Id);
				}
			}
		}
	}

	void VChatProcessPacketManager::ProcessNotifyGlobal(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize)
	{
		if( usr && usr->IsGM() && packet && packet[0])
		{
			VBuffer bufferChat = CreateBufferChat(vchat::CHAT_GLOBAL, 0);
			bufferChat.WriteBuffer(packet,nSize);
			VUserManager::Instance().SendGlobalPacket(vchat::Packet::s2c_chat_message,bufferChat.GetBuffer(),bufferChat.GetSize());
		}
	}

	void VChatProcessPacketManager::ProcessPacket(VChatUserPtr pPlayer, const ChatHeader* header, const ConstBuffer & body)
	{
		const VPacketType nPacketType = header->type;

		if (ProcessFunction[nPacketType])
		{
			(this->*ProcessFunction[nPacketType])(pPlayer, body.second, body.first);
		}
	}

	void VChatProcessPacketManager::RoomEnterMMQueue(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize)
	{
		if( usr )
		{
			usr->GetRoom()->StartMM();
			usr->GetRoom()->SendPacket(vchat::Packet::s2c_start_mm_timer, 0, 0);
		}
	}

	void VChatProcessPacketManager::RoomLeaveMMQueue(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize)
	{
		if( usr )
		{
			if (usr->GetRoom()->StopMM())
			{
				VChatRoomRef nullRoom;
				VChatRoom::WriteLogMatchMaking(usr->GetRoom(), nullRoom, 1);
				usr->GetRoom()->SendPacket(vchat::Packet::s2c_stop_mm_timer, 0, 0);
			}
		}
	}

	void VChatProcessPacketManager::ProcessKickUser(VChatUserPtr usr, const VChar *packet, const VUInt32 &nSize)
	{
		if( usr && usr->IsGM() && packet && packet[0])
		{		
			VInt32 Id = VDatabaseManager::Instance().GetRoleId(packet);
			if( Id >= 0 )
				VLobby2SocialServer::Instance().Broadcast(InternalNetwork::Packet::sc2lb_kick_role, Id);
		}
	}

	void VChatProcessPacketManager::RequestZMEFriends(VChatUserPtr pPlayer, const VChar *pData, const VUInt32 &nLen)
	{
		const vchat::Packet::REQUEST_ZM_FRIEND *pPacket = (const vchat::Packet::REQUEST_ZM_FRIEND *)pData;

		if (pPacket->nFrom >= 0 && pPacket->nFrom < pPacket->nTo)
		{
			vchat::ZME_FRIENDS zmes = vchat::VZMEManager::Instance().GetZMEFriends(pPlayer->GetPlayerID(), pPacket->nFrom, pPacket->nTo);
			VBuffer bufferZmFriends;
			if( !zmes.empty() )
			{
				VInt32 nTotalFriends = (VInt32)zmes.size();
				bufferZmFriends.WriteObject(nTotalFriends);
				for( vchat::ZME_FRIENDS::const_iterator it = zmes.begin(), e = zmes.end(); it != e; ++it)
				{
					bufferZmFriends.WriteString(it->strDisplayName);
					bufferZmFriends.WriteString(it->strURL);
					VInt32 nRole = (VInt32)it->nRoleID.size();
					bufferZmFriends.WriteObject(nRole);
					if( nRole > 0)
					{
						for( VVector<VInt32>::const_iterator itRole = it->nRoleID.begin(), eRole = it->nRoleID.end(); itRole != eRole; ++itRole )
						{
							VInt32 nRoleID = *itRole;
							VSocialDB::vchat::UserDBRef user = VSocialDB::vchat::UserTable::Instance().FindRecordEx(nRoleID);
							if( user )
							{
								VSocialDB::vchat::UserDB::RecordType userFriendRecord = user->GetRecord();
								bufferZmFriends.WriteObject(userFriendRecord.nServerID);
								bufferZmFriends.WriteObject(nRoleID);
								bufferZmFriends.WriteObject(userFriendRecord.Attr.nSex);
								bufferZmFriends.WriteObject(userFriendRecord.Attr.nLevel);
								VChatUserRef pFriend = VUserManager::Instance().GetUser(nRoleID);
								VInt32 nOnlineStatus = pFriend ? 1 : 0;
								bufferZmFriends.WriteObject(nOnlineStatus);
								bufferZmFriends.WriteString(userFriendRecord.strRoleName);
							}
						}
					}
				}
			}
			pPlayer->SendPacket(vchat::Packet::s2c_request_zme_friends_result, bufferZmFriends.GetBuffer(), bufferZmFriends.GetSize());
		}
	}

	void VChatProcessPacketManager::CodeZME(VChatUserPtr pPlayer, const VChar *pData, const VUInt32 &nLen)
	{
		const vchat::Packet::CODE_ZM *pPacket = (const vchat::Packet::CODE_ZM *)pData;

		time_t rawtime = time(NULL);
		tm *pTimeInfo = localtime(&rawtime);
		boost::format fmter("%04d%02d%02d");
		fmter % (1900 + pTimeInfo->tm_year) % (pTimeInfo->tm_mon + 1) % pTimeInfo->tm_mday;
		VString strSecretCode = md5(fmter.str().c_str());

		vchat::VZMEManager::Instance().RequestZMEFriends(pPacket->strCode, strSecretCode, pPlayer->GetServerID(), pPlayer->GetPlayerID());
	}

	void VChatProcessPacketManager::RequestZCBillingURL(VChatUserPtr pPlayer, const VChar *pData, const VUInt32 &nLen)
	{
		const vchat::Packet::REQUEST_BILLING *pPacket = (const vchat::Packet::REQUEST_BILLING *)pData;

		VString strDatas = "accountid=" + boost::lexical_cast<VString>(pPacket->nZingID) + "&accountname=" + pPacket->strAccountName + "&itemid=" + boost::lexical_cast<VString>(pPacket->nItemID);
		vchat::VZMEManager::Instance().RequestZCBillingURL(pPlayer->GetServerID(), pPlayer->GetPlayerID(), strDatas);
	}

	void VChatProcessPacketManager::SendBillingURL(const VInt32 &nPlayerID, const VString &strBillingURL)
	{
		VBuffer bufferBillingURL;
		bufferBillingURL.WriteString(strBillingURL);

		vchat::VChatUserRef pPlayer = vchat::VUserManager::Instance().GetUser(nPlayerID);
		if (pPlayer)
			pPlayer->SendPacket(vchat::Packet::s2c_request_zcredit_billing_url_result, bufferBillingURL.GetBuffer(), bufferBillingURL.GetSize());
	}

};
