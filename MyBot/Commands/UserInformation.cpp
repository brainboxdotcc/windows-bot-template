#include "UserInformation.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

namespace commands {

    dpp::slashcommand register_user_information_command(dpp::cluster& bot) {
        dpp::slashcommand command;
        command.set_name("user information")
            .set_application_id(bot.me.id)
            .set_type(dpp::ctxm_user); // Context menu for user
        return command;
    }

    void handle_user_information_command(dpp::cluster& bot, const dpp::user_context_menu_t& event) {
        std::cout << "Handling Show Avatar command" << std::endl;

        dpp::user user = event.get_user();
        std::cout << "User: " << user.username << std::endl;

        dpp::snowflake guild_id = event.command.guild_id;

        bot.guild_get_member(guild_id, user.id,
            [&bot, event, user, guild_id](const dpp::confirmation_callback_t& response) {
                if (!response.is_error()) {
                    const dpp::guild_member& member = std::get<dpp::guild_member>(response.value);
                    std::string nickname = member.get_nickname();
                    if (nickname.empty()) {
                        nickname = "None";
                    }

                    std::time_t creation_time = static_cast<std::time_t>(user.get_creation_time());
                    std::tm tm_buf;
                    char buffer[64];
                    localtime_s(&tm_buf, &creation_time);
                    std::strftime(buffer, 64, "%m/%d/%Y %I:%M:%S %p", &tm_buf);

                    std::string avatar_url = user.get_avatar_url(1024, dpp::i_png);

                    std::time_t join_time = member.joined_at;
                    char join_buffer[64];
                    localtime_s(&tm_buf, &join_time);
                    std::strftime(join_buffer, 64, "%m/%d/%Y %I:%M:%S %p", &tm_buf);

                    bot.guild_get(guild_id, [&bot, event, user, nickname, buffer, join_buffer, &guild_id, member, avatar_url](const dpp::confirmation_callback_t& guild_response) {
                        if (!guild_response.is_error()) {
                            const dpp::guild& guild = std::get<dpp::guild>(guild_response.value);
                            std::stringstream role_names;
                            for (const auto& role_id : member.get_roles()) {
                                role_names << "<@&" << role_id << ">, ";
                            }

                            std::string role_names_str = role_names.str();
                            if (!role_names_str.empty()) {
                                role_names_str.erase(role_names_str.length() - 2);
                            }

                            dpp::embed embed;
                            embed.set_title("User Info - " + nickname)
                                .add_field("Discord Name", user.username, true)
                                .add_field("Nickname", nickname, true)
                                .add_field("Account Created On", buffer, true)
                                .add_field("Joined Server On", join_buffer, true)
                                .add_field("Roles", role_names_str.empty() ? "None" : role_names_str, true)
                                .set_image(avatar_url)
                                .set_color(0x00ff00);

                            event.reply(dpp::message().add_embed(embed));
                        }
                        });
                }
            });
    }
}
