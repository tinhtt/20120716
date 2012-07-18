#include "VChat.h"
#include "VNetworkProcessPacketManager.h"
#include "VChatProcessPacketManager.h"
#include "VDatabaseManager.h"
#include "VGuildProcessPacket.h"

VNetwork::VProcessPacket* VNetwork::VProcessPacket::sProcessPacket = 0;

VNetworkProcessPacketManager::VNetworkProcessPacketManager(void)
{
}

VNetworkProcessPacketManager::~VNetworkProcessPacketManager(void)
{
}

VBool VNetworkProcessPacketManager::ReceivePacket(ConnectionPtr conn, VUserPtr user,const HeaderPtr header,const ConstBuffer &buff)
{		
	VMutableBuffer packet;
	const ChatHeader *pHeader = (const ChatHeader *)header;
	if( user )
	{
		if( !user->CanReceivePacket() || user->GetSession().get() != conn )
			return false;
	}
	else if( pHeader->type != vchat::Packet::c2s_db_login )
		return false;

	packet.first = buff.first;
	packet.second = (VChar*)(buff.second);
	return OnReceive(conn, (vchat::VChatUserPtr )user, pHeader, packet, VTrue);
}

VBool VNetworkProcessPacketManager::ReceivePacket(ConnectionPtr conn, VUserPtr user, const HeaderPtr header,const ConstBuffer &buff1,const ConstBuffer &buff2)
{
	VMutableBuffer packet;
	const ChatHeader *pHeader = (const ChatHeader *)header;
	if( user )
	{
		if( !user->CanReceivePacket() || user->GetSession().get() != conn )
			return false;
	}
	else if( pHeader->type != vchat::Packet::c2s_db_login )
		return false;

	packet.first = pHeader->l;
	packet.second = (char*)VAllocateManager::Instance().Allocate((VInt32)packet.first);

	memcpy(packet.second, buff1.second, buff1.first);
	memcpy(packet.second + buff1.first, buff2.second, buff2.first);

	return OnReceive(conn, (vchat::VChatUserPtr )user, pHeader, packet, VFalse);
}

VBool VNetworkProcessPacketManager::OnReceive( SessionPtr pSession,vchat::VChatUserPtr pPlayer, const ChatHeader* pHeader, const VMutableBuffer & buff, const VBool &bOwnMem)
{
	if (pPlayer )
	{
		if(  pPlayer->CheckSecurity(pHeader->crc, pHeader->seq, buff.second, buff.first) == VFalse)
		{
			if( buff.second && !bOwnMem)			
				VAllocateManager::Instance().DeAllocate(buff.second, (VInt32)buff.first);
			return VFalse;
		}

		if (pHeader->type > vchat::Packet::c2s_gs_unknown && pHeader->type < vchat::Packet::c2s_gs_count)
		{
			vchat::VChatProcessPacketManager::Instance().ProcessPacket(pPlayer, pHeader, ConstBuffer(buff.first, buff.second));
			if( buff.second && !bOwnMem)			
				VAllocateManager::Instance().DeAllocate(buff.second, (VInt32)buff.first);			
		}
		else if (pHeader->type > vchat::Packet::c2s_db_unknown && pHeader->type < vchat::Packet::c2s_db_count)
		{				
			VDatabaseManager::Instance().ProcessPacket(pPlayer,pHeader->type, buff.second, buff.first);			
		}
		else if(pHeader->type > vchat::Packet::c2s_guild_unknow && pHeader->type < vchat::Packet::c2s_guild_count)
		{				
			VGuildProcessPacket::Instance().ProcessPacket(pPlayer,pHeader->type, buff.second, buff.first, !bOwnMem);			
		}
	}
	else
	{			
		if (pHeader->type == vchat::Packet::c2s_db_login)
		{
			VDatabaseManager::Instance().ProcessLogin(pSession,  buff.second, buff.first);
			if( buff.second && !bOwnMem)		
				VAllocateManager::Instance().DeAllocate(buff.second, (VInt32)buff.first);			
		}	
	}
	
	return VTrue;
}
