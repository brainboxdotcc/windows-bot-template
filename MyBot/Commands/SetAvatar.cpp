#include "SetAvatar.h"
#include <dpp/dpp.h>
#include <iostream>
#include "db_access.h"

namespace commands {

    dpp::slashcommand register_setavatar_command(dpp::cluster& bot) {
        dpp::slashcommand setavatar_command("setavatar", "Set the bot's avatar", bot.me.id);
        setavatar_command.add_option(
            dpp::command_option(dpp::co_string, "url", "URL of the new avatar image", true)
        );
        // Return the command object
        return setavatar_command;
    }
    
    void handle_setavatar_command(const dpp::slashcommand_t& event, dpp::cluster& bot) {
        dpp::snowflake bot_owner_id = get_bot_owner_id(); // Fetch the owner ID

        if (event.command.usr.id != bot_owner_id) {
            // If the user is not the owner, reply with an error message and return
            event.reply("This command can only be used by the bot's owner.");
            return;
        }

        std::string image_url = std::get<std::string>(event.get_parameter("url"));

        // Fetch the image from the URL
        event.from->creator->request(image_url, dpp::http_method::m_get, [event](const dpp::http_request_completion_t& http) {
            if (http.status == 200) {
                // Update the bot's avatar with raw image data
                event.from->creator->current_user_edit("", http.body, dpp::i_png, [event](const dpp::confirmation_callback_t& confirm) {
                    if (confirm.is_error()) {
                        // Error
                        event.reply("Failed to update avatar: " + confirm.get_error().message);
                    }
                    else {
                        // Success
                        // Capture the updated user object
                        dpp::user updated_user = std::get<dpp::user>(confirm.value);

                        // Log the response headers
                        std::cout << "Response Headers:\n";
                        for (const auto& header : confirm.http_info.headers) {
                            std::cout << header.first << ": " << header.second << "\n";
                        }

                        // Log the response body
                        std::cout << "Response Body:\n" << confirm.http_info.body << "\n";

                        event.reply("Avatar updated successfully! New avatar URL: " + updated_user.get_avatar_url());
                    }
                    });
            }
            else {
                event.reply("Failed to fetch image from URL.");
            }
            });
    }
} // namespace commands
