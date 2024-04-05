#include "UtilityCommand.h"
#include "db_access.h"
#include "command_refresh.h"
#include <chrono>

namespace commands {

	dpp::slashcommand register_utility_command(dpp::cluster& bot) {
		dpp::slashcommand utility_command("utility", "Utility commands", bot.me.id);

		utility_command.add_option(
			dpp::command_option(dpp::co_sub_command, "enable", "Enable a command")
			.add_option(dpp::command_option(dpp::co_string, "command", "The command to enable", true)
				.add_choice(dpp::command_option_choice("updateraidtime", "updateraidtime"))
				.add_choice(dpp::command_option_choice("warn", "warn"))
				.add_choice(dpp::command_option_choice("brosnanstatus", "brosnanstatus"))
				.add_choice(dpp::command_option_choice("clean", "clean"))
				.add_choice(dpp::command_option_choice("facts", "facts"))
				.add_choice(dpp::command_option_choice("grantrole", "grantrole"))
				.add_choice(dpp::command_option_choice("poll", "poll"))
				.add_choice(dpp::command_option_choice("rules", "rules"))
				.add_choice(dpp::command_option_choice("setdeletechannel", "setdeletechannel"))
				.add_choice(dpp::command_option_choice("setwarnchannel", "setwarnchannel"))
				.add_choice(dpp::command_option_choice("setmemberjoinchannel", "setmemberjoinchannel"))
				.add_choice(dpp::command_option_choice("createrolesbutton", "createrolesbutton"))
				.add_choice(dpp::command_option_choice("fun", "fun"))


			));
		utility_command.add_option(
			dpp::command_option(dpp::co_sub_command, "disable", "Disable a command")
	        .add_option(dpp::command_option(dpp::co_string, "command", "The command to disable", true)
				.add_choice(dpp::command_option_choice("updateraidtime", "updateraidtime"))
				.add_choice(dpp::command_option_choice("warn", "warn"))
				.add_choice(dpp::command_option_choice("brosnanstatus", "brosnanstatus"))
				.add_choice(dpp::command_option_choice("clean", "clean"))
				.add_choice(dpp::command_option_choice("facts", "facts"))
				.add_choice(dpp::command_option_choice("grantrole", "grantrole"))
				.add_choice(dpp::command_option_choice("poll", "poll"))
				.add_choice(dpp::command_option_choice("rules", "rules"))
				.add_choice(dpp::command_option_choice("setdeletechannel", "setdeletechannel"))
				.add_choice(dpp::command_option_choice("setwarnchannel", "setwarnchannel"))
				.add_choice(dpp::command_option_choice("setmemberjoinchannel", "setmemberjoinchannel"))
				.add_choice(dpp::command_option_choice("createrolesbutton", "createrolesbutton"))
				.add_choice(dpp::command_option_choice("fun", "fun"))
		));

		utility_command.set_default_permissions(dpp::p_manage_guild);

		return utility_command;
	}

	void handle_utility_command(const dpp::slashcommand_t& event, dpp::cluster& bot) {
		// Fetch the guild owner's ID from the database
		dpp::snowflake guild_owner_id = get_guild_owner_id(event.command.guild_id);

		if (event.command.usr.id != guild_owner_id) {
			event.reply("Only the guild owner can enable or disable commands.");
			return;
		}

		std::string sub_command_group = event.command.get_command_interaction().options[0].name;

		auto last_used = get_last_used_time(event.command.usr.id, sub_command_group);
		auto now = std::chrono::system_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - last_used).count();

		if (diff < 30) { // Cooldown period of 30 seconds
			// Simplified cooldown message
			event.reply("This command is on cooldown. Please wait for 30 seconds before using it again.");
			return;
		}

		std::string command_name = std::get<std::string>(event.command.get_command_interaction().options[0].options[0].value);

		set_command_enabled_or_disabled_for_guild(event.command.guild_id, command_name, sub_command_group == "enable");

		// Update cooldown
		update_cooldown(event.command.usr.id, sub_command_group);

		refresh_guild_commands(bot, event.command.guild_id);

		event.reply("Command `" + command_name + "` has been " + (sub_command_group == "enable" ? "enabled" : "disabled") + ".");
	}
}