#pragma once
#include "../VCommon/VCommonDefines.h"

class VUser
{	
public:
	enum STATE
	{
		UNKNOW,
		LOGIN,
		LOGOUT,
	};

public:
	VUser(void);
	virtual ~VUser(void);
	static VString GetSessionAddress(SessionRef);
	static VString GetSessionAddressEx(SessionPtr);
	static void SendData(SessionPtr, const ConstBuffer &header,const ConstBuffer &buff);
public:
	SessionRef GetSession()
	{		
		return m_Session.lock();
	}
	void SetSession(SessionPtr session);
	void SetSessionEx(SessionPtr session, UserRef user);
	void SetSessionInternal(SessionPtr session);
	void SendData(const ConstBuffer &header,const ConstBuffer &buff);
	void SendBufferPtr(const VSimpleBufferPtr &buffer);
	virtual void CloseSession()
	{
		SetSession(0);
	}
	virtual void SetState(const STATE &newState)
	{
		VCommon::ScopedLock lock(m_mutex);
		m_State = newState;
		if( m_State == LOGOUT)
		{
			m_Session.reset();
		}
	}
	STATE GetCurrentState()
	{
		VCommon::ScopedLock lock(m_mutex);
		return m_State;
	}
	bool CanProcessPacket()
	{
		return CanReceivePacket();
	}
	bool CanReceivePacket()
	{
		VCommon::ScopedLock lock(m_mutex);		
		return (m_State == LOGIN) ? true :false;
	}	
	void Reset()
	{
		VCommon::ScopedLock lock(m_mutex);		
		m_State = UNKNOW;

	}
	void InitSecKey(const VUInt32 &nKey);
	VBool CheckSecurity(const VUInt32 &nCRC, const VUInt32 & nSeq, const void *, const VUInt32&);	
	void SetSeq(const VUInt32 &nSeq)
	{
		VCommon::ScopedLock lock(m_mutex);	
		m_nSeq = nSeq;
	}
protected:
	SessionWeakPtr m_Session;	
	STATE m_State;
	VCommon::Mutex m_mutex;
	VUInt32 m_CrcTable[256];
	VUInt32 m_nSeq;
	VUInt32 m_nKey;
};

