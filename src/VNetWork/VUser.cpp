#include "VUser.h"
#include "../VCommon/crc32.h"
#include "VAsyncServer.h"

VString VUser::GetSessionAddress(VSessionRef refSession)
{
	static const VString emptyAddr = "0.0.0.0";
	return refSession ? refSession->get_address() : emptyAddr;
}

VString VUser::GetSessionAddressEx(VSessionPtr pSession)
{
	static const VString emptyAddr = "0.0.0.0";
	return pSession ? pSession->get_address() : emptyAddr;
}

void VUser::SendData(VSessionPtr pSession, const VConstBuffer &header,const VConstBuffer &buff)
{
	if (pSession)
		pSession->send_packet(header, buff);

}

VUser::VUser(void)
	: m_State(UNKNOW)
	, m_nSeq(0)
{}

VUser::~VUser(void)
{}

VSessionRef VUser::GetSession() {		
	return m_Session.lock();
}

void VUser::CloseSession() {
	SetSession(0);
}

void VUser::SetState(const STATE &newState)
{
	vcommon::ScopedLock lock(m_Mutex);
	m_State = newState;
	if (m_State == LOGOUT)
		m_Session.reset();
}

VUser::STATE VUser::GetState()
{
	vcommon::ScopedLock lock(m_Mutex);
	return m_State;
}

//VBool VUser::CanProcessPacket() {
//	return CanReceivePacket();
//}
//
//VBool VUser::CanReceivePacket() {
//	vcommon::ScopedLock lock(m_Mutex);		
//	return (m_State == LOGIN) ? true :false;
//}

void VUser::Reset()
{
	vcommon::ScopedLock lock(m_Mutex);		
	m_State = UNKNOW;
}

void VUser::SendData(const VConstBuffer &header,const VConstBuffer &buff)
{
	if (VSessionRef refSession = m_Session.lock())
		refSession->send_packet(header, buff);
}

void VUser::SendBuffer(const VSimpleBufferRef &buffer)
{
	if (VSessionRef refSession = m_Session.lock())
		refSession->send_buffer(buffer);
}

void VUser::SetSession(VSessionPtr pSession)
{
	vcommon::ScopedLock lock(m_Mutex);
	SetSessionInternal(pSession);
}

void VUser::SetSessionExternal(VSessionPtr pSession, VUserRef refUser)
{
	if (pSession)
	{
		m_Session = pSession->shared_from_this();
		pSession->set_user(refUser);
	}
	else
		m_Session.reset();
}

void VUser::SetSessionInternal(VSessionPtr pSession)
{	
	if (pSession)
		m_Session = pSession->shared_from_this();	
	else
		m_Session.reset();
}

void VUser::InitSecretKey(const VUInt32 &nKey)
{
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
	if (nSeq == m_nSeq)
	{
		if (nLen == 0 || nCRC == GetCRC(m_nKey, pBuff, nLen, m_CrcTable))
		{
			++m_nSeq;
			return VTrue;
		}
	}
	return VFalse;
}