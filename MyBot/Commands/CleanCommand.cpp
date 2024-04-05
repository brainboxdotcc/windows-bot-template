#include "CleanCommand.h"

namespace commands {

    dpp::slashcommand register_clean_command(dpp::cluster& bot) {
        dpp::slashcommand clean_command("clean", "Clean messages", bot.me.id);
        clean_command.add_option(
            dpp::command_option(dpp::co_integer, "count", "Number of messages to remove", true)
        );
        clean_command.add_option(
            dpp::command_option(dpp::co_mentionable, "member", "Remove messages from this member only", false)
        );
        return clean_command;
    }

    auto handle_clean_command(const dpp::slashcommand_t& event, dpp::cluster& bot) -> dpp::task<void> {
        co_await event.co_thinking(true); // Optionally, you can pass true to make it ephemeral

        // Check permissions (assuming this part works as expected)
        if (!event.command.get_resolved_permission(event.command.usr.id).can(dpp::p_manage_messages)) {
            co_await event.co_edit_original_response(dpp::message("You do not have permission to use this command.").set_flags(dpp::m_ephemeral));
            co_return;
        }

        // Retrieve options
        int64_t count = std::get<int64_t>(event.get_parameter("count"));
        dpp::snowflake member_id = std::holds_alternative<dpp::snowflake>(event.get_parameter("member")) ? std::get<dpp::snowflake>(event.get_parameter("member")) : dpp::snowflake(0);

        // Fetch messages
        auto messages_result = co_await bot.co_messages_get(event.command.channel_id, 0, 0, 0, count);
        if (messages_result.is_error()) {
            co_await event.co_edit_original_response(dpp::message("Failed to fetch messages.").set_flags(dpp::m_ephemeral));
            co_return;
        }
        auto messages = std::get<dpp::message_map>(messages_result.value);

        // Filter messages based on criteria
        std::vector<dpp::snowflake> message_ids;
        for (const auto& [id, msg] : messages) {
            if (member_id == 0 || msg.author.id == member_id) {
                message_ids.push_back(id);
            }
        }

        // If there are messages to delete
        if (!message_ids.empty()) {
            co_await bot.co_message_delete_bulk(message_ids, event.command.channel_id);

            // Send the "Messages cleaned." message and await the confirmation
            dpp::message update_message = dpp::message(event.command.channel_id, "Messages cleaned.").set_flags(dpp::m_ephemeral);
            co_await event.co_edit_original_response(update_message);

            // Wait for 10 seconds before proceeding to delete the confirmation message
            co_await bot.co_sleep(10);

        }
        else {
            // No messages to delete, reply directly
            dpp::message no_messages_message = dpp::message(event.command.channel_id, "No messages to clean.").set_flags(dpp::m_ephemeral);
            co_await event.co_edit_original_response(no_messages_message);
        }
    }
}
