#include "button_click_handler.h"
#include <algorithm> // Include for std::find

void setup_button_click_handler(dpp::cluster& bot) {
    bot.on_button_click([&bot](const dpp::button_click_t& event) {
        if (event.custom_id == "rules_button") {
            // Acknowledge the button click and show the user an ephemeral message with role buttons
            dpp::message msg = dpp::message(event.command.channel_id, "Select a role:")
                .add_component(dpp::component()
                    .set_type(dpp::cot_action_row)
                    .add_component(dpp::component()
                        .set_type(dpp::cot_button)
                        .set_label("Subs")
                        .set_style(dpp::cos_primary)
                        .set_id("role_button_891890698041847818")) // Manually insert the role ID for "Subs" here
                    .add_component(dpp::component()
                        .set_type(dpp::cot_button)
                        .set_label("Raiders")
                        .set_style(dpp::cos_primary)
                        .set_id("role_button_891884205565431829")) // Manually insert the role ID for "Raiders" here
                    .add_component(dpp::component()
                        .set_type(dpp::cot_button)
                        .set_label("EW Savage")
                        .set_style(dpp::cos_primary)
                        .set_id("role_button_958544951862513694"))) // Manually insert the role ID for "EW Savage" here
                .set_flags(dpp::m_ephemeral);

            event.reply(dpp::ir_channel_message_with_source, msg);
        }
        });

    bot.on_button_click([&bot](const dpp::button_click_t& event) {
        if (event.custom_id.starts_with("role_button_")) {
            // Extract the role ID from the button's custom ID
            dpp::snowflake role_id = std::stoull(event.custom_id.substr(12));

            // Check if the user already has the role
            auto member_roles = event.command.member.get_roles();
            if (std::find(member_roles.begin(), member_roles.end(), role_id) != member_roles.end()) {
                // User has the role, remove it
                bot.guild_member_delete_role(event.command.guild_id, event.command.member.user_id, role_id, [&bot, event](const dpp::confirmation_callback_t& cc) {
                    if (!cc.is_error()) {
                        event.reply(dpp::message("Role removed successfully.").set_flags(dpp::m_ephemeral));
                    }
                    else {
                        auto error_info = cc.get_error();
                        std::string error_message = "Failed to remove role: " + error_info.message;
                        event.reply(dpp::message(error_message).set_flags(dpp::m_ephemeral));
                        bot.log(dpp::loglevel::ll_error, error_message);
                    }
                    });
            }
            else {
                // User doesn't have the role, add it
                bot.guild_member_add_role(event.command.guild_id, event.command.member.user_id, role_id, [&bot, event](const dpp::confirmation_callback_t& cc) {
                    if (!cc.is_error()) {
                        event.reply(dpp::message("Role added successfully.").set_flags(dpp::m_ephemeral));
                    }
                    else {
                        auto error_info = cc.get_error();
                        std::string error_message = "Failed to add role: " + error_info.message;
                        event.reply(dpp::message(error_message).set_flags(dpp::m_ephemeral));
                        bot.log(dpp::loglevel::ll_error, error_message);
                    }
                    });
            }
        }
        });

}
