#pragma once
#include "VNetWorkFwd.h"

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

	static VString GetSessionAddress(VSessionRef refSession);
	static VString GetSessionAddressEx(VSessionPtr pSession);
	static void SendData(VSessionPtr, const VConstBuffer &header,const VConstBuffer &buff);

public:

	virtual void CloseSession();
	virtual void SetState(const STATE &newState);

	STATE GetState();
	VSessionRef GetSession();
	void SetSession(VSessionPtr pSession);
	void SetSessionExternal(VSessionPtr pSession, VUserRef refUser);
	void SetSessionInternal(VSessionPtr pSession);

	void SendData(const VConstBuffer &header,const VConstBuffer &buff);
	void SendBuffer(const VSimpleBufferRef &buffer);
	
	//VBool CanProcessPacket();
	//VBool CanReceivePacket();
	VBool CheckSecurity(const VUInt32 &nCRC, const VUInt32 & nSeq, const void *, const VUInt32&);

	void Reset();
	void InitSecretKey(const VUInt32 &nKey);
	void SetSequenceNumber(const VUInt32 &nSeq);

protected:
	STATE	m_State;
	VSessionWeakRef m_Session;
	vcommon::Mutex	m_Mutex;
	VUInt32 m_CrcTable[256];
	VUInt32 m_nSeq;
	VUInt32 m_nKey;
};

