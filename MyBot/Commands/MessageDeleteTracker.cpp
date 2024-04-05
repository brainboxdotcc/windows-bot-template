#include <dpp/dpp.h>
#include <chrono>
#include "MessageDeleteTracker.h"
#include "db_access.h"

namespace commands {

    dpp::slashcommand register_setdeletechannel_command(dpp::cluster& bot) {
        dpp::slashcommand setdeletechannel_command("setdeletechannel", "Set the channel for deleted messages", bot.me.id);
        setdeletechannel_command.add_option(dpp::command_option(dpp::co_channel, "channel", "Choose a channel", true));

        // Direct registration moved to MyBot.cpp to allow command handling setup
        return setdeletechannel_command;
    }

    void setup_message_delete_tracking(dpp::cluster& bot, dpp::cache<dpp::message>& message_cache) {
        bot.on_message_create([&message_cache, &bot](const dpp::message_create_t& event) {
            // Create a copy of the message on the heap
            dpp::message* msg_copy = new dpp::message(event.msg);
            // Store the pointer in the cache
            message_cache.store(msg_copy);
            });

        bot.on_message_delete([&message_cache, &bot](const dpp::message_delete_t& event) {
            dpp::message* deleted_message = message_cache.find(event.id);
            if (deleted_message != nullptr) {
                dpp::embed embed;
                embed.set_author(deleted_message->author.username, "", deleted_message->author.get_avatar_url())
                    .set_color(0xFF0000) // Red color
                    .add_field("Deleted Message Info", "**Message sent by <@" + std::to_string(deleted_message->author.id) + "> in <#" + std::to_string(deleted_message->channel_id) + "> has been deleted.**", false);

                if (!deleted_message->content.empty()) {
                    embed.add_field("Content", deleted_message->content, false);
                }

                bool hasImageAttachment = std::any_of(deleted_message->attachments.begin(), deleted_message->attachments.end(), [](const dpp::attachment& att) {
                    return att.url.find(".png") != std::string::npos || att.url.find(".jpg") != std::string::npos || att.url.find(".jpeg") != std::string::npos || att.url.find(".gif") != std::string::npos;
                    });

                if (hasImageAttachment) {
                    dpp::embed_footer footer;
                    footer.set_text("An image was deleted with this post, attempting to upload below, Message ID: " + std::to_string(deleted_message->id));
                    embed.set_footer(footer);
                }

                embed.set_timestamp(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

                // Query the database to get the correct channel ID for this guild
                uint64_t channel_id = get_message_delete_channel_id_for_guild(deleted_message->guild_id);

                if (channel_id > 0) { // Ensure a valid channel was found
                    bot.message_create(dpp::message(channel_id, embed), [deleted_message, hasImageAttachment, &bot, channel_id](const dpp::confirmation_callback_t& response) {
                        if (hasImageAttachment) {
                            for (auto& attachment : deleted_message->attachments) {
                                if (!attachment.url.empty()) {
                                    bot.message_create(dpp::message(channel_id, attachment.url));
                                }
                            }
                        }
                        });
                }
                else {
                    std::cerr << "No delete channel set for guild: " << deleted_message->guild_id << std::endl;
                }

                message_cache.remove(deleted_message);
            }
            });
        
        bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
            if (event.command.get_command_name() == "setdeletechannel") {
                // Fetch the user's permissions in the guild
                if (event.command.get_resolved_permission(event.command.usr.id).can(dpp::p_manage_guild)) {

                // Check if the user has the permission to manage the guild
                    dpp::snowflake channel_id = std::get<dpp::snowflake>(event.get_parameter("channel"));
                    dpp::snowflake guild_id = event.command.guild_id;
                    std::string guild_name = event.command.get_guild().name;

                    // Perform your logic to set the message delete channel here
                    set_message_delete_channel(guild_id, guild_name, channel_id); // Adjust your function call accordingly

                    event.reply("Delete channel set successfully.");
                }
                else {
                    event.reply("You do not have permission to use this command.");
                }
            }
            });

        bot.on_message_delete_bulk([&message_cache, &bot](const dpp::message_delete_bulk_t& event) {
            uint64_t channel_id = 0; // This will be set based on the first found message
            bool channel_id_set = false;

            int message_count = 1;
            for (auto id : event.deleted) {
                dpp::message* deleted_message = message_cache.find(id);
                if (deleted_message != nullptr) {
                    if (!channel_id_set) {
                        // Retrieve the correct channel ID for this guild from the database
                        // This assumes all messages in the bulk delete event are from the same guild
                        channel_id = get_message_delete_channel_id_for_guild(deleted_message->guild_id);
                        if (channel_id == 0) {
                            std::cerr << "No delete channel set for guild: " << deleted_message->guild_id << std::endl;
                            return; // Early exit if no channel is configured for the guild
                        }
                        channel_id_set = true;
                    }

                    dpp::embed embed;
                    embed.set_author(deleted_message->author.username, "", deleted_message->author.get_avatar_url())
                        .set_color(0xFF0000) // Red color
                        .add_field("Bulk message deleted", "Message " + std::to_string(message_count) + " of " + std::to_string(event.deleted.size()), false)
                        .add_field("Content", deleted_message->content, false)
                        .set_timestamp(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

                    // Send the embed to the dynamically determined channel ID
                    bot.message_create(dpp::message(channel_id, embed));

                    message_cache.remove(deleted_message); // Remove the message from cache after processing

                    ++message_count;
                }
            }

            if (channel_id_set && event.deleted.size() > 0) {
                std::cout << "Processed bulk message delete. Messages deleted: " << event.deleted.size() << std::endl;
            }
            else {
                std::cerr << "Bulk delete event with no accessible messages in cache or no channel configured." << std::endl;
            }
            });

        bot.on_message_update([&message_cache](const dpp::message_update_t& event) {
            dpp::message* cached_message = message_cache.find(event.msg.id);
            if (cached_message != nullptr) {
                // Update the content of the cached message
                cached_message->content = event.msg.content;
                // Update any other relevant fields of the cached message
            }
            });
    }
}