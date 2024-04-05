#include "WarnMember.h"
#include "GuildCommands.h"
#include "db_access.h"

namespace commands {

    dpp::slashcommand register_warn_member_command(dpp::cluster& bot) {
        dpp::slashcommand register_warn_member_command("warn", "Warn a member", bot.me.id);
        register_warn_member_command.add_option(dpp::command_option(dpp::co_user, "member", "The guild member", true));
        register_warn_member_command.add_option(dpp::command_option(dpp::co_string, "reason", "The role to grant or revoke", false));
        // Return the command object
        return register_warn_member_command;
    }

    dpp::slashcommand register_setwarnchannel_command(dpp::cluster& bot) {
        dpp::slashcommand setwarnchannel_command("setwarnchannel", "Set the channel for warnings", bot.me.id);
        setwarnchannel_command.add_option(dpp::command_option(dpp::co_channel, "channel", "The channel to set for warnings", true));
        return setwarnchannel_command;
    }

    void handle_warn_member(const dpp::slashcommand_t& event, dpp::cluster& bot) {
        dpp::snowflake guild_id = event.command.guild_id; // Declare and initialize guild_id here
        dpp::snowflake bot_owner_id = get_bot_owner_id(); // Fetch the owner ID

        // Check if the warn command is enabled for this guild
        if (!is_command_enabled_for_guild(guild_id, "warn")) {
            event.reply("The warn command is disabled for this guild.");
            return;
        }

        if (event.command.usr.id != bot_owner_id) {
            // If the user is not the owner, reply with an error message and return
            event.reply("This command can only be used by the bot's owner.");
            return;
        }

        auto member = std::get<dpp::snowflake>(event.get_parameter("member"));

        std::string reason = "No reason provided"; // Default reason

        if (std::holds_alternative<std::string>(event.get_parameter("reason"))) {
            reason = std::get<std::string>(event.get_parameter("reason"));
        }

        // Fetch warn_channel_id dynamically based on the guild_id from the event
        dpp::snowflake warn_channel_id = get_warn_channel_id_for_guild(guild_id); // This function needs to be implemented

        dpp::embed warnEmbed;
        warnEmbed.set_color(dpp::colors::red)
            .set_title("Warning Issued")
            .add_field("Member Warned", "<@" + std::to_string(member) + ">", false);
        if (!reason.empty()) {
            warnEmbed.add_field("Reason", reason, false);
        }

        bot.message_create(dpp::message(warn_channel_id, warnEmbed), [event](const dpp::confirmation_callback_t& callback) {
            if (callback.is_error()) {
                event.reply("Failed to send warning message.");
            }
            else {
                event.reply("Warning issued successfully.");
            }
            });
    }

    void handle_setwarnchannel(const dpp::slashcommand_t& event, dpp::cluster& bot) {
        dpp::snowflake guild_id = event.command.guild_id;
        dpp::snowflake channel_id = std::get<dpp::snowflake>(event.get_parameter("channel"));

        // Check if the user executing the command is the guild owner
        dpp::snowflake owner_id = get_guild_owner_id(guild_id); // Implement this function to fetch the owner ID from the guild_info table
        if (event.command.usr.id != owner_id) {
            event.reply("This command can only be used by the guild's owner.");
            return;
        }

        // Set the warn channel in the database
        set_warn_channel_for_guild(guild_id, channel_id); // Implement this function to update the warn_channel_ids table

        event.reply("Warn channel set successfully.");
    }
}
