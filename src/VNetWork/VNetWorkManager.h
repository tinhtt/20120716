#pragma once
#include "VAsyncServer.hpp"

class VNetWorkManager
{
private:
	VNetWorkManager(void) {}
	~VNetWorkManager(void) {}

public:
	static VNetWorkManager &Instance() 
	{ 
		static VNetWorkManager instance;
		return instance; 
	}

	void Start(const size_t& max_session, const size_t& recycled_session, const unsigned short& port);
	void Stop();

private:
	VAsyncServerRef m_Server;
};

