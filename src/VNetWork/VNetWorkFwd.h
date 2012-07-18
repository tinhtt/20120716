#pragma once
#include "../VCommon/VCommonDefines.h"

namespace boost
{ 
	namespace asio
	{
		class session;
		class async_server;
		struct header;

		typedef session*					session_ptr;
		typedef boost::shared_ptr<session>	session_ref;
	};
};

typedef boost::asio::header* VHeaderPtr;
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