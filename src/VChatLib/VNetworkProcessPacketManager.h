#pragma once
#include "VChatUser.h"

class VNetworkProcessPacketManager: public VNetwork::VProcessPacket,public VSingleton<VNetworkProcessPacketManager>
{
public:
	VNetworkProcessPacketManager(void);
	~VNetworkProcessPacketManager(void);

	void Start() {}
	void Stop() {}
public:
	VBool OnReceive(SessionPtr pSession, vchat::VChatUser* user, const ChatHeader* pHeader, const MutableBuffer &buff, const VBool &);
	VBool ReceivePacket( SessionPtr, UserPtr, HeaderPtr, const ConstBuffer&);
	VBool ReceivePacket( SessionPtr, UserPtr, HeaderPtr, const ConstBuffer&, const ConstBuffer&);
};
