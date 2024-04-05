#include "SetPresence.h"
#include "db_access.h"

namespace commands {

    dpp::slashcommand register_setpresence_command(dpp::cluster& bot) {
        dpp::slashcommand setpresence_command("setpresence", "Set the bot's presence", bot.me.id);
        setpresence_command.add_option(
            dpp::command_option(dpp::co_string, "status", "The new status of the bot", true)
            .add_choice(dpp::command_option_choice("Online", "online"))
            .add_choice(dpp::command_option_choice("Idle", "idle"))
            .add_choice(dpp::command_option_choice("Do Not Disturb", "dnd"))
        );
        setpresence_command.add_option(
            dpp::command_option(dpp::co_string, "activity_type", "Type of activity", false)
            .add_choice(dpp::command_option_choice("Game", "game"))
            .add_choice(dpp::command_option_choice("Listening", "listening"))
            .add_choice(dpp::command_option_choice("Watching", "watching"))
            .add_choice(dpp::command_option_choice("Custom", "custom"))
        );
        setpresence_command.add_option(
            dpp::command_option(dpp::co_string, "custom_status", "Custom status text", false)
        );
        // Return the command object
        return setpresence_command;
    }
    
    void handle_setpresence_command(const dpp::slashcommand_t& event, dpp::cluster& bot) {
        dpp::snowflake bot_owner_id = get_bot_owner_id(); // Fetch the owner ID

        if (event.command.usr.id != bot_owner_id) {
            // If the user is not the owner, reply with an error message and return
            event.reply("This command can only be used by the bot's owner.");
            return;
        }

        std::string status = std::get<std::string>(event.get_parameter("status"));

        std::string activity_type_str = "game"; // Default to game
        if (event.get_parameter("activity_type").index() != 0) {
            activity_type_str = std::get<std::string>(event.get_parameter("activity_type"));
        }

        std::string custom_status = "";
        if (event.get_parameter("custom_status").index() != 0) {
            custom_status = std::get<std::string>(event.get_parameter("custom_status"));
        }

        dpp::presence_status new_status = dpp::ps_online; // Default to online
        if (status == "idle") {
            new_status = dpp::ps_idle;
        }
        else if (status == "dnd") {
            new_status = dpp::ps_dnd;
        }

        dpp::activity_type activity_type = dpp::at_game; // Default to game
        if (activity_type_str == "listening") {
            activity_type = dpp::at_listening;
        }
        else if (activity_type_str == "watching") {
            activity_type = dpp::at_watching;
        }
        else if (activity_type_str == "custom") {
			activity_type = dpp::at_custom;
		}

        bot.set_presence(dpp::presence(new_status, activity_type, custom_status));

        event.reply("Presence updated successfully.");
    }
}