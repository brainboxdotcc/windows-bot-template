#pragma once
#include <dpp/dpp.h>
#include <string>
#include <vector>
#include "mysql.h"

extern MYSQL* conn;
extern const std::chrono::seconds cooldown_duration;

void execute_sql(const std::string& sql);
void execute_sql_with_params(const std::string& sql, const std::vector<std::string>& params);
bool open_db();
bool is_command_enabled_for_guild(dpp::snowflake guild_id, const std::string& command);
void close_db();
void update_guild_info(dpp::snowflake guild_id, const std::string& guild_name, dpp::snowflake owner_id, int member_count);
void update_guild_channel_info(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string& channel_name, const std::string& channel_type);
void update_guild_role_info(dpp::snowflake guild_id, dpp::snowflake role_id, const std::string& role_name);
void remove_guild_info(dpp::snowflake guild_id);
void add_guild_rule(dpp::snowflake guild_id, const std::string& rule);
void remove_guild_rule(dpp::snowflake guild_id, int rule_number);
void set_message_delete_channel(dpp::snowflake guild_id, const std::string& guild_name, dpp::snowflake channel_id);
void update_cooldown(dpp::snowflake user_id, const std::string& command);
void remove_poll_info(dpp::snowflake guild_id);
void set_warn_channel_for_guild(dpp::snowflake guild_id, dpp::snowflake channel_id);
void set_command_enabled_or_disabled_for_guild(dpp::snowflake guild_id, const std::string& command, bool enabled);
void guild_commands_populater(dpp::snowflake guild_id, const std::string& command, bool enabled);
dpp::snowflake get_warn_channel_id_for_guild(dpp::snowflake guild_id);
dpp::snowflake get_bot_owner_id();
dpp::snowflake get_guild_owner_id(dpp::snowflake guild_id);
dpp::snowflake get_message_delete_channel_id_for_guild(dpp::snowflake guild_id);
std::string get_bot_token_from_db();
std::vector<std::string> get_guild_rules(dpp::snowflake guild_id);
std::optional<std::tuple<dpp::snowflake, dpp::snowflake>> get_poll_info(dpp::snowflake guild_id);
std::string get_guild_name(dpp::snowflake guild_id);
int get_blame_count();
std::chrono::system_clock::time_point get_last_used_time(dpp::snowflake user_id, const std::string& command);
void store_user_join_date(dpp::snowflake guild_id, dpp::snowflake user_id, int64_t join_date);
std::string get_user_join_date(dpp::snowflake guild_id, dpp::snowflake user_id);
void set_member_join_channel_for_guild(dpp::snowflake guild_id, dpp::snowflake channel_id);
dpp::snowflake get_member_join_channel_for_guild(dpp::snowflake guild_id);
std::string get_openai_api_key();
dpp::snowflake get_rules_channel_id(dpp::snowflake guild_id);