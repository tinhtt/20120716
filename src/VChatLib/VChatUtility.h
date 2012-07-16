#include "VCommonDefines.h"
#include "VUtility.h"

#define GUILD_STATUS_DELETE 255
#define NUMBER_GUILD_JUNIOR 3

static VBuffer CreateBufferChat(const VInt32& nChatType,const VInt32& nPlayerID)
{
	VBuffer buffer;
	VInt32	nType = nChatType;
	VInt32	nID = nPlayerID;
	buffer.WriteObject(nType);
	buffer.WriteObject(nID);
	return buffer;
}