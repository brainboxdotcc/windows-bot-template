#include "MemberJoinHandler.h"
#include <iostream>
#include <chrono>
#include "db_access.h"

namespace commands {

    dpp::slashcommand register_setmemberjoinchannel_command(dpp::cluster& bot) {
        dpp::slashcommand memberjoin_channel_command("setmemberjoinchannel", "The channel to send join and leave embeds", bot.me.id);
        memberjoin_channel_command.set_default_permissions(dpp::permissions::p_administrator);
        memberjoin_channel_command.add_option(dpp::command_option(dpp::co_channel, "channel", "The channel to send join and leave embeds", true));

        return memberjoin_channel_command;
    }

    void setup_memberjoin_handler(dpp::cluster& bot) {
        bot.on_guild_member_add([&bot](const dpp::guild_member_add_t& event) {
            // Check if a channel has been set for the guild in the guild_member_join_channels table
            dpp::snowflake join_channel_id = get_member_join_channel_for_guild(event.added.guild_id);
            if (join_channel_id != 0) {
                // Retrieve the user object
                dpp::user* user = event.added.get_user();

                // Retrieve the guild name from the database
                std::string guild_name = get_guild_name(event.added.guild_id);

                // Calculate the age of the account in days
                auto account_creation_time = std::chrono::system_clock::from_time_t(user->get_creation_time());
                auto now = std::chrono::system_clock::now();
                auto account_age_days = std::chrono::duration_cast<std::chrono::hours>(now - account_creation_time).count() / 24;

                if (account_age_days < 1) {
                    // Send a direct message to the user
                    bot.direct_message_create(user->id, dpp::message(event.added.user_id, "Welcome " + guild_name + "! Unfortunately, your account is less than 1 day old and is not allowed on this server.Please try again later."), [event, &bot, user](const dpp::confirmation_callback_t& cc) {
                        if (!cc.is_error()) {
                            // Remove the user from the server
                            bot.guild_member_delete(event.added.guild_id, event.added.user_id);
                        }
                        else {
                            std::cerr << "Error sending direct message: " << cc.get_error().message << std::endl;
                        }
                        });
                }
                else {
                    // Send a direct message to the user
                    bot.direct_message_create(user->id, dpp::message(event.added.user_id, "Welcome " + guild_name + "! Please read the rules in the rules channel and click the button to agree to the rules and assign yourself a role. Only after this will you be able to use text and voice channels."));

                    int64_t join_timestamp = event.added.joined_at;

                    store_user_join_date(event.added.guild_id, user->id, join_timestamp);

                    dpp::embed welcome_embed;
                    welcome_embed.set_author(user->username, "", user->get_avatar_url());
                    welcome_embed.set_footer("User ID: " + std::to_string(user->id), "");

                    int64_t creation_time = static_cast<int64_t>(user->get_creation_time());
                    int64_t now_timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

                    // Use the guild name in plain text instead of a mention, fetched from the database
                    std::string guild_name = get_guild_name(event.added.guild_id);

                    welcome_embed.set_description(
                        "User <@" + std::to_string(user->id) + "> joined " + guild_name +
                        " at <t:" + std::to_string(now_timestamp) + ":f>. User's account was created <t:" + std::to_string(creation_time) + ":f>."
                    );

                    // Send the welcome embed to the designated channel
                    bot.message_create(dpp::message(join_channel_id, "").add_embed(welcome_embed), [/* lambda capture */](const dpp::confirmation_callback_t& response) {
                        if (response.is_error()) {
                            std::cerr << "Error sending embed: " << response.get_error().message << std::endl;
                        }
                        });
                }
            }
            });

        bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
            if (event.command.get_command_name() == "setmemberjoinchannel") {
                // Fetch the user's permissions in the guild
                if (event.command.get_resolved_permission(event.command.usr.id).can(dpp::p_manage_guild)) {

                    // Check if the user has the permission to manage the guild
                    dpp::snowflake channel_id = std::get<dpp::snowflake>(event.get_parameter("channel"));
                    dpp::snowflake guild_id = event.command.guild_id;

                    // Perform your logic to set the message delete channel here
                    set_member_join_channel_for_guild(event.command.guild_id, channel_id);

                    event.co_reply("Member join channel set to <#" + std::to_string(channel_id) + ">.");
                }
                else {
                    event.reply("You do not have permission to use this command.");
                }
            }
            });

        bot.on_guild_member_remove([&bot](const dpp::guild_member_remove_t& event) {
            // Check if a channel has been set for the guild in the guild_member_join_channels table
            dpp::snowflake join_channel_id = get_member_join_channel_for_guild(event.guild_id);
            if (join_channel_id != 0) {
                std::string join_date = get_user_join_date(event.guild_id, event.removed.id);

                // Construct the avatar URL
                std::string avatar_url = event.removed.get_avatar_url();

                // Prepare the embed for the member leaving
                dpp::embed leave_embed;
                leave_embed.set_author(event.removed.username, avatar_url, avatar_url);

                leave_embed.set_description(
                    "User <@" + std::to_string(event.removed.id) + "> left the guild. " +
                    "They joined the guild on <t:" + join_date + ":f>."
                );

                // Send the leave embed to the designated channel
                bot.message_create(dpp::message(join_channel_id, "").add_embed(leave_embed));
            }
            });
    }

}