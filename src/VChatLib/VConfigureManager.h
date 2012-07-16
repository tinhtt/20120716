#pragma once
#include "VCommonDefines.h"

namespace vchat
{
	enum DB_CONFIG_NAME
	{
		DB_CONFIG_SOCIAL,
		DB_CONFIG_GUILD,
		DB_CONFIG_GUILD_GLOBAL,
		DB_CONFIG_ACCOUNT,
		DB_CONFIG_COUNT
	};

	enum MM_CONFIG
	{
		MM_CONFIG_NUM_PLAYERS = 5,
		MM_CONFIG_NUM_DELTA = 5,
	};

	struct VConfigDB
	{
		VString m_StrDBName;
		VString m_StrDBUser;
		VString m_strDBPass;
		VString m_strDBServer;
	};

	class VConfigureManager: public VSingleton<VConfigureManager>
	{
	public:
		VConfigDB m_cfg[DB_CONFIG_COUNT];

		VString	m_strConfigFile;
		size_t	m_nMaxSession;
		size_t	m_nRecycleSession;
		VUInt16	m_nPort;
		VInt32	m_nMaxPlayer;
		VInt32	m_nMaxRoom;	
		VInt32	m_nIntervalDBUpdate;
		VInt32	m_nChangeUpdate;
		VString	m_strLogAddr;
		VUInt16	m_nSocialPort;
		VInt32	m_nLimitMemberInGuild;
		VInt32	m_nLimitRequestJoinGuild;
		VInt32	m_nMatchMakingInterval;
		VInt32	m_nMatchMakingDelta[MM_CONFIG_NUM_DELTA];
		VInt32	m_nMinMMRoom[MM_CONFIG_NUM_PLAYERS];
		VString m_strChatAddr;
		VString m_strChatPort;
		VString m_strWebServiceAddr;
		VString m_strZMGetFriendFile;
		VString m_strZCreditFile;
		VString m_strSGNProcessFile;
		VInt32 m_nGuildRechargeAssetRate;

	public:
		VConfigureManager(void);
		~VConfigureManager(void);

	private:
		void Start();
	};
}