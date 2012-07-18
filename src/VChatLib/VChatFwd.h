#pragma once
namespace vchat
{
	class VChatUser;
	typedef VChatUser*	VChatUserPtr;
	typedef VSharedPtr<VChatUser>	VChatUserRef;
	typedef VWeakPtr<VChatUser>		VChatUserWRef;

	class VChatRoom;
	typedef VChatRoom*	VChatRoomPtr;
	typedef VSharedPtr<VChatRoom>	VChatRoomRef;
	typedef VWeakPtr<VChatRoom>		VChatRoomWRef;
};