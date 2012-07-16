#include "VChat.h"
#include "VSystemManager.h"
#include "../VGameDB/SocialDBManager.h"
#include "VLogManager.h"
#include "VDatabaseManager.h"
#include "../VNetWork/VNetWorkManager.h"
#include "VUserManager.h"
#include "VConfigureManager.h"
#include "VNetworkProcessPacketManager.h"
#include "VChatUtility.h"
#include "VLobby2SocialProcessPacket.h"
#include "../VGuildLib/VGuildProcessPacket.h"
#include "VMMProcessManager.h"
#include "VHttpServerManager.h"
#include "VHttpProcessManager.h"
#include "VZMEManager.h"
#include "VXMLManager.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <dirent.h>
#include <csignal>
#endif

VSystemManager::VSystemManager(void)
{
}

VSystemManager::~VSystemManager(void)
{
}

void VSystemManager::Start( void )
{
	vchat::VXMLManager::Instance().Start();
	boost::asio::VIntenalNetWork::VAsyncIo::Instance();
	const vchat::VConfigureManager &cfg = vchat::VConfigureManager::Instance();	
	VLogManager::Instance().Start(cfg.m_strLogAddr.c_str());
	SocialDBManager::Start(cfg.m_cfg, cfg.m_nIntervalDBUpdate, cfg.m_nChangeUpdate);
	VDatabaseManager::Instance().Start();
	vchat::VUserManager::Instance().Start();
	vchat::VMMProcessManager::Instance().Start(cfg.m_nMatchMakingInterval);
	VHttpServerManager::Instance().Start<vchat::VHttpProcessManager>(cfg.m_strChatAddr.c_str(), cfg.m_strChatPort.c_str());
	vchat::VZMEManager::Instance().Start();
	VGuildProcessPacket::Instance().Start();
	VLobby2SocialServer::Instance().Start(cfg.m_nSocialPort);
	VNetworkProcessPacketManager::Instance().Start();
	VNetWorkManager::Instance().Start(cfg.m_nMaxSession, cfg.m_nRecycleSession, cfg.m_nPort);
}

void VSystemManager::Stop( void )
{
	boost::asio::VIntenalNetWork::VAsyncIo::Instance().Stop();
	VNetWorkManager::Instance().Stop();
	VNetworkProcessPacketManager::Instance().Stop();
	VLobby2SocialServer::Instance().Stop();
	VGuildProcessPacket::Instance().Stop();
	vchat::VZMEManager::Instance().Stop();
	VHttpServerManager::Instance().Stop();
	vchat::VMMProcessManager::Instance().Stop();
	vchat::VUserManager::Instance().Stop();
	VDatabaseManager::Instance().Stop();
	SocialDBManager::Stop();	
	VLogManager::Instance().Stop();
	vchat::VXMLManager::Instance().Stop();
}

#ifndef _WIN32
static VBool g_bRun = VTrue;
void SignalHandler(VInt32 nSigNum)
{
    g_bRun = VFalse;
}
#endif
    
void ChatStart(VChar *pPath)
{
        VChar szPath[128] = {0};

#ifdef _WIN32
        VChar *pSep = strrchr(pPath, '\\');
        if (pSep)
        {
            strncpy(szPath, pPath, pSep - pPath + 1);
            _chdir(szPath);
        }
#else
        VChar *pSep = strrchr(pPath, '/');
        if (pSep)
        {
            strncpy(szPath, pPath, pSep - pPath + 1);
            chdir(szPath);
        }
#endif

	VSystemManager::Instance().Start();	
	std::cout << "VChatApp has started" << std::endl;

#ifdef _WIN32
	VChar szBuffer[256] = {0};
	std::memset(szBuffer, 0, sizeof(szBuffer));
	while (std::cin.getline(szBuffer, 255))
	{
		if (szBuffer[0] == 'q' || szBuffer[0] == 'Q') break;
		if (szBuffer[0] == 's' || szBuffer[0] == 'S' && szBuffer[1])
		{
			VBuffer bufferChat = CreateBufferChat(vchat::CHAT_GLOBAL, 0);
			bufferChat.WriteBuffer(&szBuffer[1],64);
			vchat::VUserManager::Instance().SendGlobalPacket(vchat::Packet::s2c_chat_message,bufferChat.GetBuffer(),bufferChat.GetSize());
		}
		std::memset(szBuffer, 0, sizeof(szBuffer));
	}
#else
        std::signal(SIGTERM, &SignalHandler);
        while (g_bRun) sleep(1);
#endif

	VSystemManager::Instance().Stop();
	std::cout << "VChatApp has stopped" << std::endl;
}

