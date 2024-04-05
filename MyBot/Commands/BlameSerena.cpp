#include "BlameSerena.h"
#include "db_access.h" // Include your database access functions
#include <iostream> // Include for logging
#include <chrono> // Include for time calculations

namespace commands {

    dpp::slashcommand register_blameserena_command(dpp::cluster& bot) {
        dpp::slashcommand blameserena_command("blameserena", "Blame Serena!", bot.me.id);
        bot.global_command_create(blameserena_command);

        return blameserena_command;
    }

    void handle_blameserena_command(const dpp::slashcommand_t& event, dpp::cluster& bot) {
        std::cout << "Handling /blameserena command." << std::endl;

        // Get the last used time for the blameserena command
        auto last_used = get_last_used_time(event.command.usr.id, "blameserena");
        auto now = std::chrono::system_clock::now();
        auto cooldown_end = last_used + cooldown_duration;

        // Check if the user is currently on cooldown
        if (now < cooldown_end) {
            std::cout << "User " << event.command.usr.id << " is on cooldown." << std::endl;

            // Calculate the Unix timestamp when the cooldown will end
            auto cooldown_end_timestamp = std::chrono::duration_cast<std::chrono::seconds>(cooldown_end.time_since_epoch()).count();

            // Create a message object with the cooldown information and set it as ephemeral
            dpp::message cooldown_message("You are blaming Serena too quickly. Please try again at <t:" + std::to_string(cooldown_end_timestamp) + ":t>.");
            cooldown_message.set_flags(dpp::m_ephemeral);

            // Send the ephemeral message
            event.reply(cooldown_message);
            return;
        }
        else {
            // If not on cooldown, update the cooldown for this user and command
            update_cooldown(event.command.usr.id, "blameserena");

            // Increment the blame count in the database
            execute_sql("UPDATE blame_count SET count = count + 1 WHERE id = 1;");

            // Fetch the updated count
            int blame_count = get_blame_count();

            // Update the bot's status
            std::string status_text = "Serena Blamed " + std::to_string(blame_count) + " Times";
            bot.set_presence(dpp::presence(dpp::ps_online, dpp::at_custom, status_text));

            // Reply to acknowledge the command with the updated blame count
            event.reply("Serena has been blamed " + std::to_string(blame_count) + " Times.");
        }
    }
}