#pragma once
#include "VCommonDefines.h"
namespace vguild
{
	namespace Packet
	{
#pragma pack(push, 1)
		enum ERROR_PACKET_RESULT
		{
			GUILD_COMMAND_RESULT_SUCCESS,
			GUILD_COMMAND_RESULT_FAIL,
			CREATE_GUILD_PACKET_RESULT_SUCCESS,
			CREATE_GUILD_PACKET_RESULT_FAIL,
			CREATE_GUILD_PACKET_RESULT_EXIST_NAME,
			GET_GUILD_INFO_PACKET_RESULT_FAIL,
			REQUEST_JOIN_GUILD_RESULT_SUCCESS,
			REQUEST_JOIN_GUILD_RESULT_FAIL,
			MEMBER_ACCEPT_INVITE_RESULT_SUCCESS,
			MEMBER_ACCEPT_INVITE_RESULT_FAIL,
			LEAVE_RESULT_SUCCESS,
			KICK_RESULT_SUCCESS,
			ACCEPT_INVITE_RESULT_SUCCESS,
			UPDATE_ANNOUNCE_RESULT_SUCCESS,
			ACCEPT_MEMBER_RESULT_FAIL,
			OWN_GUILD_LEAVE_RESULT_SUCCESS,
			OVERLOAD_MEMBER_IN_GUILD_RESULT,
			OVERLOAD_REQUEST_JOIN_GUILD_RESULT,
			PROMOTE_GUILD_JUNIOR_OVERLOAD_JUNIOR,
			PROMOTE_GUILD_JUNIOR_SUCCESS,
			FIRE_GUILD_JUNIOR_SUCCESS,
			FIRE_GUILD_JUNIOR_FAIL,
			FIRE_GUILD_JUNIOR_YOU_ARE_FIRED,
			FIRE_GUILD_JUNIOR_YOU_ARE_PROMOTED,
			GUILD_ACTIVE_SKILL_SUCCESS,
			GUILD_ACTIVE_SKILL_FAIL,
			GUILD_BUY_ITEM_SUCCESS,
			GUILD_BUY_ITEM_FAIL,
			GUILD_RECHARGE_ASSET_SUCCESS,
			GUILD_STORE_UP_LEVEL_SUCCESS,
			GUILD_STORE_UP_LEVEL_FAIL,
			GUILD_STORE_UP_LEVEL_GUILD_NOT_ENOUGH_EXP,
			GUILD_STORE_UP_LEVEL_GUILD_NOT_ENOUGH_ASSET
		};

		enum TYPE_PROCESS_JUNIOR
		{
			PROMOTE,
			FIRE
		};

		struct CREATE_GUILD_PACKET
		{
			VChar szGuildName[17];//16
			VInt32 nFlagID;
			VBool bMethodChecking;
			CREATE_GUILD_PACKET()
			{
				memset(szGuildName, 0, 17);
				nFlagID = 0;
			}
		};

		struct GET_GUILD_INFO_PACKET
		{
			VInt32 nFrom;
			VInt32 nTo;
		};

		struct GET_LIST_GUILD_PACKET
		{
			VInt32 nFrom;
			VInt32 nTo;
		};

		struct GUILD_UPDATE_ANNOUNCE
		{
			VChar szMessage[193];//192
		};

		struct GUILD_COMMAND_PACKET
		{
			VInt32 nType;		// 0: invite, 1: kick, 2: accept, 3: reject, 4: leave, 5: request join, 6: get info, 7: accept invitation, 8: total member, 9: total request, 10: total guild, 11: promote guild junior, 12: get info for profile, 13: fire guild junior.
			VInt32 nID;
		};

		struct GET_REQUEST_LIST_PACKET
		{
			VInt32 nFrom;
			VInt32 nTo;
		};

		struct REQUEST_FRIEND_IN_GUILD
		{
			VInt32 nFrom;
			VInt32 nTo;
			VBool  bOnlineOnly;
		};

		struct GUILD_STORE_UP_LEVEL
		{
			VInt32	nGuildStoreType;
			VInt32	nLevelUp;
		};
#pragma pack(pop)
	};
};
namespace vchat
{
	namespace Packet
	{
		#pragma pack(push, 1)
		enum ADD_FRIEND_RESULT
		{
			ADD_FRIEND_RESULT_FAIL,
			ADD_FRIEND_RESULT_SUCCESS,
			ADD_FRIEND_RESULT_AVAILABLE
		};

		enum DELETE_FRIEND_RESULT
		{
			DELETE_FRIEND_RESULT_SUCCESS,
			DELETE_FRIEND_RESULT_FAIL
		};

		typedef struct
		{
			VInt32 nPlayerID;
			VInt32 nModeID;
			VInt32 nRoomID;
			VInt32 nMissionID;
		} INVITE_JOIN_ROOM_PACKET;

		typedef struct
		{
			VUInt8 nResult;
		} RESULT_PACKET;

		typedef struct
		{
			VChar Name[33];
		} NAME_PACKET;

		typedef struct
		{
			VInt32	nPlayerID;
		} ADD_FRIEND_BY_ID_PACKET;

		typedef struct
		{
			VUInt32 nRoleID;
			VInt8 nSex;
			VUInt32 nLevel;
			VUInt8 nServerID;
			VChar Name[33];
			VChar AccName[33];
		} LOGIN_PACKET;

		typedef struct  
		{
			VInt32 nFrom;
			VInt32 nTo;
			VBool  bOnlineOnly;
		} PLAYER_LIST_REQUEST;

		typedef struct
		{
			VChar Msg[32];
		} CHAT_MESSAGE_PACKET;
		
		typedef struct
		{
			VInt32 nLevel;
		} UPDATE_LEVEL_PACKET;

		typedef struct
		{
			VChar AccName[33];
			VChar Msg[65];
		} CHAT_PRIVATE_MESSAGE_PACKET;

		typedef struct
		{
			VInt32 nPlayerID;
		} PLAYER_PACKET;

		typedef struct
		{
			VInt32	nRoomID;
			VInt8	nRoomMode;
		} JOIN_ROOM_PACKET;

		typedef struct
		{
			VInt32 nFrom;
			VInt32 nTo;
		} REQUEST_ZM_FRIEND;

		typedef struct  
		{
			VChar strCode[256];
		} CODE_ZM;

		typedef struct  
		{
			VInt32 nZingID;
			VChar strAccountName[34];
			VInt32 nItemID;
		} REQUEST_BILLING;

		#pragma pack(pop)

		class Login : public Base
		{
		public:
			void *GetBuff()
			{
				return Name;
			}
			UINT_32 GetSize()
			{
				return strlen(Name) + 1;
			}		
			/*void Destroy()
			{
				ObjectPtrManager<Packet::Login>::Instance().DestroyObject(this);
			}*/
			bool CheckSize(size_t l)
			{
				return l > 0 && l <= 32;
			}
			const char *GetName()
			{
				return Name;
			}
		public:
			char Name[32];
		};
		class ChatRoom : public Base
		{
		public:
			void *GetBuff()
			{
				return Msg;
			}
			UINT_32 GetSize()
			{
				return strlen(Msg) + 1;
			}
			/*void Destroy()
			{
				ObjectPtrManager<Packet::ChatRoom>::Instance().DestroyObject(this);
			}*/
			bool CheckSize(size_t l)
			{
				return l > 0 && l <= 32;
			}
		public:
			char Msg[64];
		};
		class JoinRoom : public Base
		{
		public:
			void *GetBuff()
			{
				return &RoomId;
			}
			UINT_32 GetSize()
			{
				return sizeof(RoomId);
			}
			bool CheckSize(size_t l)
			{
				return l == GetSize();
			}
			/*void Destroy()
			{
				ObjectPtrManager<Packet::JoinRoom>::Instance().DestroyObject(this);
			}*/
		public:
			int RoomId;
		};
	};
};
