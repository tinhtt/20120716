#pragma once
#include "../VCommon/VCommonDefines.h"

namespace boost
{ 
	namespace asio
	{
		class session;
		class async_server;
		struct Header;
	};
};

typedef boost::asio::Header* VHeaderPtr;

typedef std::pair<size_t, const char*>	VConstBuffer;
typedef std::pair<size_t, char*>		VMutableBuffer;

typedef VSharedPtr<boost::asio::async_server> VAsyncServerRef;

typedef boost::asio::session*				VSessionPtr;
typedef VSharedPtr<boost::asio::session>	VSessionRef;
typedef VWeakPtr<boost::asio::session>		VSessionWeakRef;

class VUser;	
typedef VUser* VUserPtr;
typedef VSharedPtr<VUser>	VUserRef;
typedef VWeakPtr<VUser>		VUserWeakRef;

namespace vnetwork
{
	class VProcessPacket
	{		
	public:
		VProcessPacket() {
			sProcessPacket = this;
		}

		virtual ~VProcessPacket() {}

		virtual VBool ReceivePacket(VSessionPtr, VUserPtr, VHeaderPtr, const VConstBuffer&) = 0;
		
		static VProcessPacket *sProcessPacket;
	};
};