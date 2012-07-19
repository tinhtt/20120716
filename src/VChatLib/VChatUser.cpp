#include "VChatUser.h"

namespace vchat
{
	VChatUser::VChatUser(void) : m_nServerID(0), m_nKey(0), m_nSeq(0), m_bGM(VFalse) {}

	VChatUser::~VChatUser(void) {}

	/*void VChatUser::SendData( SessionPtr pSession, const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen )
	{
		ChatHeader header;
		header.type = nPacketType;
		header.l = nLen;
		VUser::SendData(pSession, ConstBuffer(sizeof(header), (const char*)&header), ConstBuffer(nLen,(const char*)pPacket));
	}*/
	
	void VChatUser::SendPacket(const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen)
	{
		VChatHeader header;
		header.type	= nPacketType;
		header.len	= nLen;
		VUser::SendData(VConstBuffer(sizeof(header), (const char*)&header), VConstBuffer(nLen,(const char*)pPacket));
	}

	void VChatUser::CloseSession()
	{	
		m_strUserName.clear();
		m_nPlayerID = 0;
		m_nServerID = 0;
		m_nSeq = 0;
		m_nKey = 0;
		SetState(VUser::LOGOUT);
	}
};
