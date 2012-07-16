#pragma once
#include "boost/thread.hpp"
#include "boost/smart_ptr.hpp"
#include "VNetWorkFwd.h"
class VNetWorkManager
{
private:
	VNetWorkManager(void)
	{
	}

	virtual ~VNetWorkManager(void)
	{
	}
public:
	static VNetWorkManager &Instance() 
	{ 
		static VNetWorkManager instance; 
		return instance; 
	}
	void Start(size_t max_session, size_t recycled_session, unsigned short port);
	void Stop();
private:	
	AsyncServerRef m_Server;
};

