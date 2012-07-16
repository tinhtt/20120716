#pragma once
namespace vchat
{
	namespace Packet
	{
		typedef unsigned int UINT_32;
		typedef unsigned char UINT_8;
		typedef std::pair<size_t, const char*> ConstBuffer;
		enum PacketType
		{
			c2s_gs_unknown = -1,
			c2s_gs_login,
			c2s_gs_chat_message,
			c2s_gs_join_room,	
			c2s_gs_ban_nick, //= 3
			c2s_gs_tb_global,// 4
			c2s_gs_enter_queue,
			c2s_gs_leave_queue,
			c2s_gs_kick_user,
			c2s_request_zme_friends,
			c2s_send_code_zme,
			c2s_request_zc_billing_url,
			c2s_gs_count,

			c2s_db_unknown = 1000,
			c2s_db_login,
			c2s_db_request_friend_list,
			c2s_db_add_friend,
			c2s_db_invite_join_room,
			c2s_db_delete_friend,
			c2s_db_add_black_list,
			c2s_db_add_friend_by_id,
			c2s_db_chat_message_private,
			c2s_db_update_level,
			c2s_db_request_list_player_online,
			c2s_db_count,
			c2s_guild_unknow = 2000,
			c2s_guild_create_guild,
			c2s_guild_member_list,
			c2s_guild_get_guild_list,
			c2s_guild_update_announce,
			c2s_guild_command,
			c2s_guild_get_request_list,
			c2s_guild_chat_message_guild,
			c2s_guild_request_friend_in_guild,
			c2s_guild_store_level_up,
			c2s_guild_count
		};
		
		enum s2cGuildPacketType
		{
			s2c_guild_unknown = 2000,
			s2c_guild_error_result,
			s2c_guild_member_list_result,
			s2c_guild_get_guild_list_result,
			s2c_guild_get_request_list_result,
			s2c_guild_get_info_result,
			s2c_guild_invite_result,
			s2c_guild_accept_invite_result,
			s2c_guild_accept_request_join_result,
			s2c_guild_get_total_member_result,
			s2c_guild_get_total_request_result,
			s2c_guild_get_total_guild_result,
			s2c_guild_leave_guild_result,
			s2c_guild_get_info_for_profile_result,
			s2c_guild_request_friend_in_guild_result,
			s2c_guild_send_info_player_inserted_to_everyone_result,
			s2c_guild_send_info_player_deleted_to_everyone_result,
			s2c_guild_send_info_player_inserted_to_guild_boss_result,
			s2c_guild_send_info_player_deleted_to_guild_boss_result,
			s2c_guild_type_process_junior_result,
			s2c_guild_update_announce_result,
			s2c_guild_count
		};
		enum s2cPacketType
		{
			s2c_unknown = -1,
			s2c_login_result,
			s2c_chat_message,
			s2c_friend_list,
			s2c_add_friend_result,
			s2c_invite_join_room,
			s2c_delete_friend_result,
			s2c_add_black_list_result,
			s2c_invite_join_room_result,
			s2c_chat_crc,
			s2c_list_player_online,
			s2c_join_mm_room,
			s2c_start_mm_timer,
			s2c_mm_room_found,
			s2c_stop_mm_timer,
			s2c_request_zme_friends_result,
			s2c_request_zcredit_billing_url_result,
			s2c_count
		};
#pragma pack(push, 1)
		struct Header
		{
			unsigned int l;
			unsigned int crc;
			unsigned int seq;
			unsigned short type;
		};
#pragma pack(pop)
		class Base
		{
		public:
			virtual void *GetBuff() = 0;
			virtual UINT_32 GetSize() = 0;
			virtual void Destroy() = 0;
			virtual bool CheckSize(size_t ) = 0;
		};			
		typedef Packet::Base* PacketBasePtr;
	};
};
typedef vchat::Packet::Header ChatHeader;