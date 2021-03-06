#include "VNetWorkManager.h"

void VNetWorkManager::Start(const size_t& max_session, const size_t& recycled_session, const unsigned short& port)
{
	size_t hardware_concurrency = boost::thread::hardware_concurrency();
	m_Server.reset(new boost::asio::async_server( hardware_concurrency <= 0 ? 1 : (2 * hardware_concurrency), max_session, recycled_session, port));
}

void VNetWorkManager::Stop()
{
	if (m_Server.get())
	{
		m_Server->stop();
		m_Server.reset();
	}
}
