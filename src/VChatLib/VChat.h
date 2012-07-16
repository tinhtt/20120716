#pragma once
#include "VCommonDefines.h"
#include "VChatPacket.h"
#include "../VNetWork/VInternalPacket.h"
#include "../VNetWork/VInternalNetwork.h"

namespace vchat
{
	using namespace vcommon;	
	enum
	{
		IsolationRoom = -1,
	};

	enum CHAT_TYPE
	{
		CHAT_NORMAL,
		CHAT_SYSTEM_NOTIFY,
	};
};

