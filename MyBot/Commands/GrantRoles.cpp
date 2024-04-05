#include "GrantRoles.h"

namespace commands {

    dpp::slashcommand register_grantroles_command(dpp::cluster& bot) {
        dpp::slashcommand grantroles_command("grantrole", "Grant or Revoke a Role", bot.me.id);
        grantroles_command.add_option(dpp::command_option(dpp::co_user, "member", "The guild member", true));
        grantroles_command.add_option(dpp::command_option(dpp::co_role, "role", "The role to grant or revoke", true));
        
        // Return the command object
        return grantroles_command;
    }

    void handle_grantroles_command(const dpp::slashcommand_t& event, dpp::cluster& bot) {
        bot.log(dpp::loglevel::ll_debug, "handle_grantroles_command called");
        // Check for administrative permission
        if (!(event.command.get_resolved_permission(event.command.usr.id).can(dpp::p_manage_roles))) {
            event.reply(dpp::message("You don't have sufficient permissions to use this command.").set_flags(dpp::m_ephemeral));
            return;
        }

        // Retrieve options
        dpp::snowflake member_id = std::get<dpp::snowflake>(event.get_parameter("member"));
        dpp::snowflake role_id = std::get<dpp::snowflake>(event.get_parameter("role"));

        // Fetch the guild member
        auto member = dpp::find_guild_member(event.command.guild_id, member_id);
        if (member.user_id == 0) {
            event.reply("Member not found!");
            return;
        }

        // Get the member's roles
        auto member_roles = member.get_roles();

        // Check if the member has the role, then either add or remove it
        auto role_it = std::find(member_roles.begin(), member_roles.end(), role_id);
        if (role_it != member_roles.end()) {
            // Member has the role, so remove it
            dpp::snowflake interaction_id = event.command.id; // Capture the interaction ID
            std::string token = event.command.token; // Capture the token
            bot.guild_member_delete_role(event.command.guild_id, member_id, role_id, [interaction_id, token, &bot](const dpp::confirmation_callback_t& cc) {
                bot.log(dpp::loglevel::ll_debug, "guild_member_delete_role callback invoked");
                if (!cc.is_error()) {
                    dpp::interaction_response response(dpp::ir_channel_message_with_source, dpp::message("Role removed from member.").set_flags(dpp::m_ephemeral));
                    bot.interaction_response_create(interaction_id, token, response);
                }
                else {
                    dpp::interaction_response response(dpp::ir_channel_message_with_source, dpp::message("Failed to remove role.").set_flags(dpp::m_ephemeral));
                    bot.interaction_response_create(interaction_id, token, response);
                    bot.log(dpp::loglevel::ll_debug, cc.get_error().message);
                }

                });
        }
        else {
            // Member does not have the role, so add it
            dpp::snowflake interaction_id = event.command.id; // Capture the interaction ID
            std::string token = event.command.token; // Capture the token
            bot.guild_member_add_role(event.command.guild_id, member_id, role_id, [interaction_id, token, &bot](const dpp::confirmation_callback_t& cc) {
                if (!cc.is_error()) {
                    dpp::interaction_response response(dpp::ir_channel_message_with_source, dpp::message("Role granted to member.").set_flags(dpp::m_ephemeral));
                    bot.interaction_response_create(interaction_id, token, response);
                }
                else {
                    dpp::interaction_response response(dpp::ir_channel_message_with_source, dpp::message("Failed to add role.").set_flags(dpp::m_ephemeral));
                    bot.interaction_response_create(interaction_id, token, response);
                    bot.log(dpp::loglevel::ll_debug, cc.get_error().message);
                }

                });
        }


    }

}
