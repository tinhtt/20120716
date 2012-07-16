#include "VChat.h"
#include "VConfigureManager.h"
#include "boost/program_options.hpp"

using namespace std;
namespace po = boost::program_options;

namespace vchat
{
	VConfigureManager::VConfigureManager(void):m_strConfigFile("config/chat_server.cfg")
		, m_nMaxSession(10000)
		, m_nRecycleSession(1000)
		, m_nPort(1234)
		, m_nMaxPlayer(4096 << 4)
		, m_nMaxRoom(4096)
		, m_nIntervalDBUpdate(0)
		, m_nChangeUpdate(0)
		, m_nSocialPort(6065)
		, m_nLimitMemberInGuild(0)
		, m_nLimitRequestJoinGuild(0)
		, m_nMatchMakingInterval(10000)
		, m_nGuildRechargeAssetRate(1)
	{
		Start();
	}

	VConfigureManager::~VConfigureManager(void)
	{
	}

	void VConfigureManager::Start()
	{
		VString strHttpServerAddr;
		VString strChatAddr;
		std::string strDelta;
		std::string strRoomLimit;
		po::options_description configFileOptions;
		configFileOptions.add_options()
			("MaxSession", po::value<size_t>(&m_nMaxSession))
			("RecycleSession", po::value<size_t>(&m_nRecycleSession))
			("ServerPort", po::value<VUInt16>(&m_nPort))

			("DBSocialServer", po::value<VString>(&m_cfg[DB_CONFIG_SOCIAL].m_strDBServer))
			("DBSocialName", po::value<VString>(&m_cfg[DB_CONFIG_SOCIAL].m_StrDBName))
			("DBSocialUser", po::value<VString>(&m_cfg[DB_CONFIG_SOCIAL].m_StrDBUser))
			("DBSocialPass", po::value<VString>(&m_cfg[DB_CONFIG_SOCIAL].m_strDBPass))

			("DBGlobalServer", po::value<VString>(&m_cfg[DB_CONFIG_GUILD_GLOBAL].m_strDBServer))
			("DBGlobalName", po::value<VString>(&m_cfg[DB_CONFIG_GUILD_GLOBAL].m_StrDBName))
			("DBGlobalUser", po::value<VString>(&m_cfg[DB_CONFIG_GUILD_GLOBAL].m_StrDBUser))
			("DBGlobalPass", po::value<VString>(&m_cfg[DB_CONFIG_GUILD_GLOBAL].m_strDBPass))

			("DBCenterServer", po::value<VString>(&m_cfg[DB_CONFIG_GUILD].m_strDBServer))
			("DBCenterName", po::value<VString>(&m_cfg[DB_CONFIG_GUILD].m_StrDBName))
			("DBCenterUser", po::value<VString>(&m_cfg[DB_CONFIG_GUILD].m_StrDBUser))
			("DBCenterPass", po::value<VString>(&m_cfg[DB_CONFIG_GUILD].m_strDBPass))

			("DBAccountServer", po::value<VString>(&m_cfg[DB_CONFIG_ACCOUNT].m_strDBServer))
			("DBAccountName", po::value<VString>(&m_cfg[DB_CONFIG_ACCOUNT].m_StrDBName))
			("DBAccountUser", po::value<VString>(&m_cfg[DB_CONFIG_ACCOUNT].m_StrDBUser))
			("DBAccountPass", po::value<VString>(&m_cfg[DB_CONFIG_ACCOUNT].m_strDBPass))

			("MaxPlayer", po::value<VInt32>(&m_nMaxPlayer))
			("MaxRoom", po::value<VInt32>(&m_nMaxRoom))		
			("IntervalDBUpdate", po::value<VInt32>(&m_nIntervalDBUpdate))
			("ChangeUpdate", po::value<VInt32>(&m_nChangeUpdate))
			("LogAddress", po::value<VString>(&m_strLogAddr)->default_value("127.0.0.1:6666"))
			("SocialPort", po::value<VUInt16>(&m_nSocialPort))
			("LimitMemberInGuild", po::value<VInt32>(&m_nLimitMemberInGuild))
			("LimitRequestJoinGuild", po::value<VInt32>(&m_nLimitRequestJoinGuild))

			("MatchMakingInterval", po::value<VInt32>(&m_nMatchMakingInterval))
			("MatchMakingDelta", po::value<std::string>(&strDelta))
			("MatchMakingMinRoom", po::value<std::string>(&strRoomLimit))

			("ChatAddress", po::value<VString>(&strChatAddr)->default_value("10.199.88.5:6065"))

			("WebServiceAddress", po::value<VString>(&strHttpServerAddr)->default_value("10.199.88.5:6065"))
			("ZMGetFriendFile", po::value<VString>(&m_strZMGetFriendFile))
			("ZCreditFile", po::value<VString>(&m_strZCreditFile))
			("SGNProcessFile", po::value<VString>(&m_strSGNProcessFile))

			("GuildRechargeAssetRate", po::value<VInt32>(&m_nGuildRechargeAssetRate))
			;

		ifstream inputFile(m_strConfigFile.c_str());
		if(inputFile)
		{
			po::variables_map variablesMap;
			store(parse_config_file(inputFile, configFileOptions), variablesMap);
			notify(variablesMap);
			m_cfg[DB_CONFIG_SOCIAL].m_strDBPass			+= "##";
			m_cfg[DB_CONFIG_GUILD].m_strDBPass			+= "##";
			m_cfg[DB_CONFIG_GUILD_GLOBAL].m_strDBPass	+= "##";
			m_cfg[DB_CONFIG_ACCOUNT].m_strDBPass	+= "##";

			std::vector<std::string> vDelta		= po::split_unix(strDelta, "; ");
			std::vector<std::string> vRoomLimit	= po::split_unix(strRoomLimit, "; ");
			VInt32 nSize = std::min(std::min(vDelta.size(), vRoomLimit.size()), (size_t)MM_CONFIG_NUM_PLAYERS);
			for (VInt32 i = 0; i < nSize; i++)
			{
				m_nMatchMakingDelta[i]	= boost::lexical_cast<VInt32>(vDelta[i]);
				m_nMinMMRoom[i]			= boost::lexical_cast<VInt32>(vRoomLimit[i]);
			}
			
			VInt32 nColonPosition = strChatAddr.find_first_of(":");
			m_strChatAddr = strChatAddr.substr(0, nColonPosition);
			m_strChatPort = strChatAddr.substr(nColonPosition + 1);

			m_strWebServiceAddr = "http://" + strHttpServerAddr;
		}
		else
			cout<<"Can not open config file: "<<m_strConfigFile<<", use defaut config\n";
	}
};