#include "VChat.h"
#include "VUserManager.h"
#include "VCommonDefines.h"
#include "VConfigureManager.h"
namespace vchat
{
	VUserManager::VUserManager(void)
	{
	}

	VUserManager::~VUserManager(void)
	{
	}	
	VChatUserPtr VUserManager::CreateUser( const VInt32& nUserID, SessionPtr Session, const VString &strName )
	{		
		if( GetUserCount() < VConfigureManager::Instance().m_nMaxPlayer )
		{
			VChatUserRef user = UserAllocator::Create() ;
			if( user )
			{
				user->SetSessionEx(Session, user);
				user->SetUserName(strName);
				//user->SetRoom(0);
				user->SetPlayerID(nUserID);	
				AddUser(user);
				return user.get();
			}			
		}
		return VNull;
	}
	
	VChatUserRef VUserManager::GetUser( const VInt32 &nID )
	{
		VReadLock lock(m_Mutex);
		VUserContainerConstIt it = m_pUsers.find(nID);
		return (it == m_pUsers.end()) ? VChatUserRef() : it->second;
	}

	VInt32 VUserManager::GetUserCount()
	{
		VReadLock lock(m_Mutex);
		return (VInt32)m_pUsers.size();
	}

	VBool VUserManager::CanCreateUser()
	{
		return GetUserCount() < VConfigureManager::Instance().m_nMaxPlayer;
	}	

	VBool VUserManager::IsEmpty()
	{
		return GetUserCount() <= 0;
	}

	VBool VUserManager::RemoveUser( VChatUserPtr pUser )
	{
		VWriteLock lock(m_Mutex);
		VUserContainerIt it = m_pUsers.find(pUser->GetPlayerID());
		if (it != m_pUsers.end() && it->second.get() == pUser)
		{				
			pUser->JoinRoom(NullRoom);
			pUser->Close();					
			m_pUsers.erase(it);						
			return VTrue;
		}

		return VFalse;
	}	 
	VBool VUserManager::AddUser( const VChatUserRef &pUser )
	{
		if (GetUserCount() < VConfigureManager::Instance().m_nMaxPlayer)
		{
			VWriteLock lock(m_Mutex);
			std::pair<VUserContainerIt, VBool> res = m_pUsers.insert(std::make_pair(pUser->GetPlayerID(),pUser));
			if( res.second == VFalse )
			{
				VChatUserRef pOldPlayer = res.first->second;
				res.first->second = pUser;
				pOldPlayer->JoinRoom(NullRoom);
				pOldPlayer->Close();			
				pOldPlayer->SetState(VUser::KICK);
				//m_PoolUsers.DeAllocate(pOldPlayer);

			}
			else
			{

			}		
			return VTrue;
		}

		return VFalse;	
	}

	void VUserManager::Start()
	{

	}

	void VUserManager::Stop()
	{
		
	}

	void VUserManager::SendServerPacket( const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen,const VUInt32& nServerID)
	{
		ChatHeader header;
		header.type = nPacketType;
		header.l = nLen;
		VSimpleBufferPtr buffer = boost::make_shared<VSimpleBuffer>(&header, sizeof(header), pPacket, nLen);
		if( !buffer->isValid() )
			return;
		VReadLock lock(m_Mutex);
		for (VUserContainerIt it = m_pUsers.begin(), e = m_pUsers.end();it != e; it++)
		{			
			if (it->second->GetServerID() == nServerID)
			{
				it->second->SendBufferPtr(buffer);
			}
		}
	}

	void VUserManager::SendGlobalPacket( const VPacketType &nPacketType, const void *pPacket, const VUInt32 &nLen )
	{
		VReadLock lock(m_Mutex);
		ChatHeader header;
		header.type = nPacketType;
		header.l = nLen;
		VSimpleBufferPtr buffer = boost::make_shared<VSimpleBuffer>(&header, sizeof(header), pPacket, nLen);
		if( !buffer->isValid() )
			return;
		//std::for_each(m_pUsers.begin(), m_pUsers.end(), boost::bind(&VChatUser::SendBufferPtr, _1, buffer));		
		for (VUserContainerIt it = m_pUsers.begin(), e = m_pUsers.end();it != e; it++)
		{
			it->second->SendBufferPtr(buffer);			
		}		
	}

	VectorInt32 VUserManager::GetUserIDArray(const VInt32 &nServerID, VInt32 nFrom, VInt32 nTo)
	{
		VectorInt32 arrPlayerID;
		VReadLock lock(m_Mutex);	
		if( nTo < 0 )
			nTo = (VInt32)m_pUsers.size();
		if( nFrom < nTo )
		{				
			arrPlayerID.reserve(nTo - nFrom);
			VInt32 i = 0;
			for (VUserContainerIt it = m_pUsers.begin(), e = m_pUsers.end();it != e ; it++)
			{
				VChatUserPtr pUser = it->second.get();
				if (pUser->GetServerID() == nServerID)
				{
					if( i >= nTo )
						break;
					else if( i >= nFrom )
						arrPlayerID.push_back(pUser->GetPlayerID());
					++i;
				}
			}
		}
		return arrPlayerID;
	}

}
