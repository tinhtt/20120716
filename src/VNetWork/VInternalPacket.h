#pragma once
#include "VPacket.h"
#ifndef MAX_USER_NAME_LEN
#define MAX_USER_NAME_LEN 32
#endif
#ifndef MAX_TANK_EQUIP_COUNT
#define MAX_TANK_EQUIP_COUNT	8
#endif
#ifndef MAX_AWARD_ITEM_COUNT
#define MAX_AWARD_ITEM_COUNT	32
#endif
#ifndef MAX_MONSTER_KILLED_COUNT
#define MAX_MONSTER_KILLED_COUNT	128
#endif
#ifndef MAX_SKILL_COUNT
#define MAX_SKILL_COUNT	32
#endif
#ifndef MAX_PLAYER_REWARD_COUNT
#define MAX_PLAYER_REWARD_COUNT	8
#endif
#ifndef MAX_ITEM_DROP_COUNT
#define MAX_ITEM_DROP_COUNT	128
#endif
#ifndef MAX_PLAYER_GUILD_SKILL_COUNT
#define MAX_PLAYER_GUILD_SKILL_COUNT 24
#endif
#include "VCommonDefines.h"
namespace InternalNetwork
{
	namespace Packet
	{
#pragma pack(push, 1)
		typedef struct
		{
			VUInt32 l;
			VPacketType type;
		} Header;
#pragma pack(pop)
		enum lobby2billing
		{
			lb2bil_get_money,
			lb2bil_consume_money,
			lb2bil_check_money,
			lb2bil_cnt
		};
		enum billing2lobby
		{
			bil2lb_money_info,
			bil2lb_buy_confirm,
			bil2lb_check_money_result,
			bil2lb_cnt
		};
		enum lobby2game
		{			
			lb2gs_create,	
			lb2gs_input,
			lb2gs_playerleavegame,
			lb2gs_start,	
			lb2gs_stop_loading,	
			lb2gs_keep_alive,
			lb2gs_revive_player,
			lb2gs_cnt
		};
		enum game2lobby
		{
			gs2lb_init,			
			gs2lb_roomcnt,
			gs2lb_ready,
			gs2lb_ingame,
			gs2lb_playertank,
			//gs2lb_changeroomstate,
			gs2lb_setroomstate,
			gs2lb_setmap,
			gs2lb_saveplayer,
			gs2lb_gotonextmap,
			gs2lb_awarditems,
			gs2lb_savemission,
			gs2lb_roomresult,
			gs2lb_roomstart,
			gs2lb_roomstart_err,
			gs2lb_player_use_item,
			gs2lb_player_rewards,
			gs2lb_save_player_items_drop,
			gs2lb_keep_alive,
			gs2lb_save_player_trophy_and_honor,
			gs2lb_revive_player_by_billing,
			gs2lb_cnt
		};
		enum social2lobby
		{
			sc2lb_unknown,
			sc2lb_create_guild_result,
			sc2lb_guild_info_result,
			sc2lb_kick_role,
			sc2lb_buy_item,
			sc2lb_active_skill,
			sc2lb_cnt
		};
		enum lobby2social
		{
			lb2sc_unknown = -1,
			lb2sc_notify,
			lb2sc_req_create_guild,
			lb2sc_request_guild_info,
			lb2sc_player_action,
			lb2sc_guild_buy_item,
			lb2sc_guild_active_skill,
			lb2sc_guild_contribute_battle_point,
			lb2sc_cnt
		};
		enum
		{
			MaxPlayerGame = 16,
		};
#pragma pack(push, 1)
		struct Lobby2GameCreate
		{
			struct PlayerInfo
			{
				VInt32							nPlayerID;			//PlayerID
				VInt32							nPlayerLevel;			//PlayerLevel
				VChar							strPlayerName[MAX_USER_NAME_LEN + 1];	//Tên của Player
				VBool							bPlayerSex;			//Giới tính của Player
				VInt32							nSelectedTankID;	//TankID mà player đã chọn		
				VInt32							nSlotID;
				VInt32							nTeamID;
				VUInt32							nKey;
				VInt32							nTankAttID;
				VInt32							nGuildID;
				VTankEquipmentDBAttributes		arrTankEquip[MAX_TANK_EQUIP_COUNT];
				VPlayerItemDBAttributes			arrPlayerItem[MAX_PLAYER_ITEM_COUNT];
				VPlayerEquipmentDBAttributes	arrPlayerEquip[MAX_PLAYER_EQUIP_COUNT];
				VInt32							nGuildSkillSize;
				VInt32							arrGuildSkill[MAX_PLAYER_GUILD_SKILL_COUNT];
			};
			VInt32 nRoomID;
			VUInt8 nPlayerCnt;
			PlayerInfo Players[MaxPlayerGame];
			VInt32 nDuration;
			VInt32 nWinScore;
			VInt32 nDifficult;
			VInt32 nModeId;
			VInt32 nMapId;
			VInt32 nMissionId;
		};
		struct Game2LobbyInit
		{		
			VUInt8 Address[4];
			VUInt16 nPortNum;
		};
		struct Game2LobbyRoomCnt
		{			
			VUInt32 nRoomCnt;
		};
		struct Game2LobbyReady
		{
			VInt32 nRoomID;
		};
		struct Lobby2GameInput
		{
			VInt32 nPlayerID;
			INPUT_PACKET Input;

		};
		struct Lobby2GamePlayerLeaveGame
		{
			VInt32 nPlayerID;
		};
		struct Game2LobbyPlayerTank
		{
			VInt32 nPlayerID;
			VInt32 nTankID;
		};
		struct Game2LobbyChangeRoomState
		{
			VInt32 nRoomID;
			VInt8 nState;
		};
		struct Game2LobbySetRoomState
		{
			VInt32 nRoomID;
			VInt8 nState;
		};
		struct Game2LobbySetNextMap
		{
			VInt32 nRoomID;
			VInt32 nMapID;
		};
		struct Game2LobbySavePlayer
		{
			VInt32 nPlayerID;
			VInt32 nSelectedTankId;
			VInt32 nExp;
			VInt32 nGold;
			VInt32 nMonsterKilled;
			VQuantityPair MonsterKilleds[MAX_MONSTER_KILLED_COUNT];
			VInt32 nSkillUsed;
			VQuantityPair SkillUseds[MAX_SKILL_COUNT];
			VInt32 nItemUsed;
			VInt32	nModeID;
			VInt32	nTankKilled;
		};
		struct Game2LobbyGotoNextMap
		{
			VInt32 nRoomID;
		};

		struct Game2LobbyRevivePlayerByBilling
		{
			VInt32		nPlayerID;
		};

		struct Game2LobbySavePlayerTrophyAndHonor
		{
			VInt32		nPlayerID;
			VInt32		nSubMode;
			VInt32		nHonor;
			VInt32		nTrophy;
		};

		struct Game2LobbySavePlayerItemsDrop
		{
			VInt32		nPlayerID;
			VInt32		nItemDropCount;
			VItemPair	arrItemDrop[MAX_ITEM_DROP_COUNT];
		};

		struct Game2LobbyPlayerReward
		{
			VInt32 nPlayerId;
			VInt32 nRewardCount;
			VInt32 arrRewardIds[MAX_PLAYER_REWARD_COUNT];
		};

		struct Game2LobbyLuckyAwards
		{
			VInt32 nRoomID;
			VInt32 nItemCnt;
			VAwardItemForPlayer Items[MAX_AWARD_ITEM_COUNT];
		};
		struct Game2LobbySaveMission
		{
			VInt32 nRoomID;
		};
		struct Game2LobbyStartError
		{
			VInt32 nRoomID;
			VInt32 nError;
			Lobby2GameCreate packet;
		};
		struct Game2LobbyPlayerUseItem
		{
			VInt32 nPlayerID;
			VInt32 nIndex;
			VInt32 nQuantity;
		};
		struct Game2LobbyRoomResult
		{
			VInt32 nRoomID;
			VInt8 nWinTeam;			
			VInt8 nPacketCnt;
			GAME_RESULT_PACKET  gamepackets[MaxPlayerGame];
		};

		struct Lobby2GameRevivePlayer
		{
			VInt32 nPlayerID;
		};

		struct Lobby2GameStart
		{
			VInt32 nRoomID;
		};
		struct Lobby2GameStopLoading
		{
			VInt32 nRoomID;
		};
		struct Game2LobbyRoomStart
		{
			VInt32 nRoomID;
		};

		struct BillingInfo
		{
			VInt32 nTotalXu;
		};

		struct Billing2LobbyMoneyInfo
		{
			VInt32		nUserId;
			BillingInfo	Info;
		};

		struct Billing2LobbySaleConfirm
		{
			VUInt8		nResult;
			VInt8		nActionType;
			VInt32		nUserId;
			VInt32		nXuBeforeBuy;
			BillingInfo Info;
			//ITEM_INFO	arrItem[MAX_BUY_ITEMS];
		};

		struct Lobby2BillingGetMoney
		{
			VInt32		nUserId;
			VChar		szAccountName[MAX_USER_NAME_LEN + 1];
		};

		struct Lobby2BillingConsumeMoney
		{
			VInt8	nActionType;
			VChar	szAccountName[MAX_USER_NAME_LEN + 1];
			VInt32	nServerId;
			VInt32	nUserId;
			VInt32	nMoney;
			VInt32	nDataSize;
			//ITEM_INFO	arrItem[MAX_BUY_ITEMS];
		};

		enum NOTIFY_TYPE
		{
			NOTIFY_TYPE_SERVER_NOTIFICATION,		
			NOTIFY_TYPE_SPEAKER,
		};

		struct Lobby2SocialPlayerAction
		{
			VInt8	nActionType;
			VInt32	nPlayerID;
			VInt32	nActionID;
		};

		struct Lobby2SocialServerNotify
		{
			VInt32	nNotifyType;
			VChar	szNotify[MAX_SERVER_NOTIFY_LEN];
		};

		struct Lobby2SocialCreateGuildInfo
		{
			VInt8	nMoneyType;
			VInt32	nServerId;
			VInt32	nUserId;
			VInt32	nFlagId;
			VChar	szGuildName[MAX_GUILD_NAME_LEN + 1];
		};

		struct Lobby2SocialActiveSkill
		{
			VInt32	nPlayerID;
			VInt32	nSkillID;
		};

		struct Lobby2SocialBuyItem
		{
			VInt32	nPlayerID;
			VInt32  nItemID;
			VInt32  nItemCategory;
			VInt32  nItemQuantity;
		};

		struct Lobby2SocialContributeGuildBattlePoint
		{
			VInt32	nPlayerID;
			VInt32	nXu;
		};

		struct Lobby2SocialRequestGuildInfo
		{
			VInt32	nPlayerID;
		};

		struct Social2LobbyGuildInfoResult
		{
			VInt32	nPlayerID;
			VInt32	nGuildID;
			VInt32	nFlagID;
			VChar	szGuildName[MAX_GUILD_NAME_LEN + 1];
		};

		struct Social2LobbyCreateGuildResult
		{
			VInt8	nResult;
			VInt8	nMoneyType;
			VInt32	nUserId;
		};
		struct Social2LobbyBanRole
		{
			VInt32	nPlayerID;
		};
#pragma pack(pop)
	};
};
