#pragma once
#include "VCommonDefines.h"
#include "VSingleton.h"
#include "VChat.h"
#include "VChatUser.h"
#include "VChatUserFwd.h"

namespace vchat
{
	class VUserManager: public VSingleton<VUserManager>
	{
	public:				
		typedef VSafeMap<VInt32, VChatUserRef> VUserContainer;
		typedef VUserContainer::iterator		VUserContainerIt;
		typedef VUserContainer::const_iterator	VUserContainerConstIt;
		typedef VObjectRefAllocator<vchat::VChatUser> UserAllocator;
	public:
		VUserManager(void);
		~VUserManager(void);
	public:
		VChatUserPtr CreateUser(const VInt32& nUserID, SessionPtr Session, const VString &strName);

		VBool RemoveUser(VChatUserPtr pUser);	

		VChatUserRef GetUser(const VInt32 &nID) ;
		VectorInt32	GetUserIDArray(const VInt32 &nServerID, VInt32, VInt32);
		VInt32 GetUserCount() ;
		VBool CanCreateUser() ;
		VBool IsEmpty() ;		
		void SendGlobalPacket(const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen);
		void SendServerPacket( const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen,const VUInt32& nServerID);
		//VBool DeleteUser( const VInt32 &nID);		
	private:				
		VBool AddUser(const VChatUserRef &pUser);
	public:
		void Start();
		void Stop();		
	private:
		VUserContainer	m_pUsers;
		//UserPoolContainer m_PoolUsers;
		VMutex m_Mutex;
	};
}