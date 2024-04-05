#include "command_refresh.h"
#include "db_access.h"
#include "CleanCommand.h"
#include "FactsCommand.h"
#include "grantroles.h"
#include "UpdateRaidTime.h"
#include "rules_command.h"
#include "MessageDeleteTracker.h"
#include "BrosnanStatus.h"
//#include "PollCommand.h"
#include "WarnMember.h"
#include "MemberJoinHandler.h"
#include "CreateRolesButton.h"
#include "FunCommand.h"

void refresh_guild_commands(dpp::cluster& bot, dpp::snowflake guild_id) {
	// Define a list of all possible guild-specific commands
	std::vector<dpp::slashcommand> all_guild_commands = {
		commands::register_clean_command(bot),
		commands::register_facts_command(bot),
		commands::register_grantroles_command(bot),
		commands::register_updateraidtime_command(bot),
		commands::register_rules_command(bot),
		commands::register_setdeletechannel_command(bot),
		commands::register_brosnanstatus_command(bot),
		//commands::register_poll_command(bot),
		commands::register_setwarnchannel_command(bot),
		commands::register_warn_member_command(bot),
        commands::register_setmemberjoinchannel_command(bot),
        commands::register_createrolesbutton_command(bot),
        commands::register_fun_command(bot)
		// Add other guild-specific commands as needed
	};

    std::cout << "Starting to populate or update the database with the current state of all commands for guild: " << guild_id << std::endl;
    for (const auto& cmd : all_guild_commands) {
        guild_commands_populater(guild_id, cmd.name, true);
    }

    std::vector<dpp::slashcommand> enabled_guild_commands;
    for (auto& cmd : all_guild_commands) {
        bool enabled = is_command_enabled_for_guild(guild_id, cmd.name);
        if (enabled) {
            enabled_guild_commands.push_back(cmd);
        }
    }
    std::cout << "Starting guild_bulk_command_create for guild: " << guild_id << std::endl;
    bot.guild_bulk_command_create(enabled_guild_commands, guild_id, [guild_id](const dpp::confirmation_callback_t& callback) {
        if (callback.is_error()) {
            std::cerr << "Error refreshing guild commands for guild " << guild_id << ": " << callback.get_error().message << std::endl;
        }
        else {
            std::cout << "Guild commands refreshed successfully for guild " << guild_id << std::endl;
        }
        });
}
