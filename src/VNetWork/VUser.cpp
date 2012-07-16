#include "VNetwork.h"
#include "VUser.h"
#include "VUtility.h"
#include "AsyncServer.h"
VUser::VUser(void) : m_State(UNKNOW), m_nSeq(0)
{
}
VUser::~VUser(void)
{
}
void VUser::SendData(const ConstBuffer &header,const ConstBuffer &buff)
{
	//VCommon::ScopedLock lock(m_mutex);
	if( SessionRef pSession = m_Session.lock() )
		pSession->send_packet(header, buff);
}
void VUser::SendBufferPtr(const VSimpleBufferPtr &buffer)
{
	if( SessionRef pSession = m_Session.lock() )
		pSession->sendbuffer(buffer);
}
VString VUser::GetSessionAddress(SessionRef session)
{
	static const VString emptyAddr = "0.0.0.0";
	return session ? session->get_address() : emptyAddr;
}
VString VUser::GetSessionAddressEx(SessionPtr session)
{
	static const VString emptyAddr = "0.0.0.0";
	return session ? session->get_address() : emptyAddr;
}
void VUser::SendData(SessionPtr session, const ConstBuffer &header,const ConstBuffer &buff)
{
	if( session )
		session->send_packet(header, buff);

}
void VUser::SetSession(SessionPtr session)
{
	VCommon::ScopedLock lock(m_mutex);
	SetSessionInternal(session);
}
void VUser::SetSessionEx(SessionPtr session, UserRef user)
{
	//VCommon::ScopedLock lock(m_mutex);	
	if( session )
	{
		m_Session = session->shared_from_this();
		session->SetUser(user);
	}
	else
		m_Session.reset();
}
void VUser::SetSessionInternal(SessionPtr session)
{	
	if( session )
		m_Session = session->shared_from_this();	
	else
		m_Session.reset();

}
void VUser::InitSecKey(const VUInt32 &nKey)
{
	//VCommon::ScopedLock lock(m_mutex);
	m_nKey = nKey;
	m_nSeq = 0;
	for (VUInt32 n = 0; n < 256; ++n) 
	{
		VUInt32 c = n;
		for (VInt32 k = 0; k < 8; ++k)
		{
			if((c & 1) != 0) c = nKey ^ (c >> 1);
			else c = c >> 1;
		}
		m_CrcTable[n] = c;
	}
}
VBool VUser::CheckSecurity(const VUInt32 &nCRC, const VUInt32 & nSeq, const void *pBuff, const VUInt32& nLen)
{
	//VCommon::ScopedLock lock(m_mutex);
	if( nSeq == m_nSeq )
	{
		if( nLen == 0 || nCRC == GetCRC(m_nKey, pBuff, nLen, m_CrcTable) )
		{
			++m_nSeq;
			return VTrue;
		}
	}
	return VFalse;
}