#pragma once
namespace vchat
{
	namespace packet
	{
		enum EPacketType
		{
			c2s_unknown = -1,
			c2s_login,
			c2s_message,
			c2s_count,
		};
		
		enum s2cPacketType
		{
			s2c_unknown = -1,
			s2c_login_result,
			s2c_message,
			s2c_count
		};

#pragma pack(push, 1)
		typedef struct
		{
			unsigned int len;
			unsigned int crc;
			unsigned int seq;
			unsigned short type;
		}  SHeader;

		typedef struct
		{
			VUInt8 nResult;
		} SResultPacket;

		typedef struct
		{
			VUInt32 nRoleID;
			VUInt32 nLevel;
			VInt8	nSex;
			VUInt8	nServerID;
			VChar	Name[33];
			VChar	AccName[33];
		} SLoginPacket;

		typedef struct
		{
			VChar Msg[32];
		} SChatMessagePacket;
#pragma pack(pop)
	};
};

typedef vchat::packet::SHeader VChatHeader;