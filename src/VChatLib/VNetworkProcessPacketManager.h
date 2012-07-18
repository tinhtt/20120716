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
	VBool OnReceive(SessionPtr pSession, vchat::VChatUser* user, const ChatHeader* pHeader, const VMutableBuffer &buff, const VBool &);
	VBool ReceivePacket( SessionPtr, VUserPtr, HeaderPtr, const ConstBuffer&);
	VBool ReceivePacket( SessionPtr, VUserPtr, HeaderPtr, const ConstBuffer&, const ConstBuffer&);
};
