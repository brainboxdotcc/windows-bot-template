#include "rules_command.h"
#include <fstream>
#include <vector>
#include "db_access.h"

namespace commands {

    dpp::slashcommand register_rules_command(dpp::cluster& bot) {
        dpp::slashcommand rules_command("rules", "Manage server rules", bot.me.id);
        // Add subcommands
        rules_command.add_option(
            dpp::command_option(dpp::co_sub_command, "add", "Add a new rule")
            .add_option(dpp::command_option(dpp::co_string, "rule", "Rule text", true))
        );
        rules_command.add_option(
            dpp::command_option(dpp::co_sub_command, "remove", "Remove a rule")
            .add_option(dpp::command_option(dpp::co_integer, "rule-number", "Rule number", true))
        );
        rules_command.add_option(
            dpp::command_option(dpp::co_sub_command, "populate", "Populate rules in a channel")
            .add_option(dpp::command_option(dpp::co_channel, "channel", "Target channel", true))
        );
        rules_command.add_option(
            dpp::command_option(dpp::co_sub_command, "repopulate", "Repopulate rules in a channel")
            .add_option(dpp::command_option(dpp::co_channel, "channel", "Target channel", true))
        );
        rules_command.add_option(
            dpp::command_option(dpp::co_sub_command, "button", "Add a button to the rules message")
            .add_option(dpp::command_option(dpp::co_channel, "channel", "Target channel", false))
            .add_option(dpp::command_option(dpp::co_string, "button-text", "Button text", false))
        );
        // Return the command object
        return rules_command;
    }

    void handle_rules_command(const dpp::slashcommand_t& event, dpp::cluster& bot) {
        std::string sub_command = event.command.get_command_interaction().options[0].name;
        bot.log(dpp::ll_info, "Received rules command with subcommand: " + sub_command);

        dpp::snowflake guild_id = event.command.guild_id;
        dpp::snowflake invoker_id = event.command.usr.id;
        dpp::snowflake owner_id = get_guild_owner_id(guild_id);

        if (invoker_id != owner_id) {
            event.reply("You must be the guild owner to use this command.");
            return;
        }

        if (sub_command == "add") {
            auto new_rule = std::get<std::string>(event.get_parameter("rule"));
            dpp::snowflake guild_id = event.command.guild_id;

            // Add rule to the database for this guild
            add_guild_rule(guild_id, new_rule);

            event.reply("Rule added successfully.");
        }

        else if (sub_command == "remove") {
            auto rule_number_param = event.get_parameter("rule-number"); // Ensure the name matches exactly

            // Use std::visit to handle the parameter safely
            std::visit([&event, &rule_number_param](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                    // If the type is int64_t, proceed to remove the rule
                    int rule_number = static_cast<int>(arg);
                    remove_guild_rule(event.command.guild_id, rule_number);
                    event.reply("Rule removed successfully.");
                }
                else {
                    // Handle unexpected type or log it
                    event.reply("Unexpected type for rule number.");
                    std::cerr << "Expected int64_t for rule number, but found other type." << std::endl;
                }
                }, rule_number_param);
        }
    
        else if (sub_command == "populate") {
            // Fetch the channel ID from the command parameter
            dpp::snowflake channel_id = std::get<dpp::snowflake>(event.get_parameter("channel"));
            // Retrieve the guild ID from the command event
            dpp::snowflake guild_id = event.command.guild_id;

            // Get rules from the database for this specific guild
            std::vector<std::string> rules = get_guild_rules(guild_id);

            // Initialize an embed to send the rules
            dpp::embed rules_embed;
            rules_embed.set_title("Server Rules");
            rules_embed.set_color(0x0000FF); // Set color to blue

            // Concatenate all rules into one string, each rule on a new line
            std::string rules_description;
            for (size_t i = 0; i < rules.size(); ++i) {
                rules_description += std::to_string(i + 1) + ". " + rules[i];
                if (i < rules.size() - 1) { // Add a blank line between rules, except after the last one
                    rules_description += "\n\n";
                }
            }

            // If there are no rules in the database for this guild
            if (rules.empty()) {
                rules_description = "No rules set for this guild.";
            }

            // Set the description for the embed with all the rules
            rules_embed.set_description(rules_description);

            // Send the embed to the specified channel
            bot.message_create(dpp::message(channel_id, rules_embed), [event](const dpp::confirmation_callback_t& response) {
                if (!response.is_error()) {
                    // If the message is sent successfully, reply to the command
                    event.reply("Rules populated successfully in the specified channel.");
                }
                else {
                    // Handle errors, e.g., if the bot doesn't have permission to send messages to the channel
                    event.reply("Failed to populate rules. Make sure I have permission to send messages to the specified channel.");
                }
                });
        }

        else if (sub_command == "repopulate") {
            dpp::snowflake channel_id = std::get<dpp::snowflake>(event.get_parameter("channel"));
            dpp::snowflake guild_id = event.command.guild_id; // Use guild_id from the command event
            bot.log(dpp::ll_info, "Repopulating rules in channel: " + std::to_string(channel_id));

            auto response = std::make_shared<dpp::slashcommand_t>(event);

            // Fetch updated rules from the database
            std::vector<std::string> rules = get_guild_rules(guild_id);

            // Retrieve messages from the channel to find the rules message
            bot.messages_get(channel_id, 0, 0, 0, 100, [response, &bot, channel_id, rules](const dpp::confirmation_callback_t& confirmation) {
                if (confirmation.is_error()) {
                    auto error_info = confirmation.get_error();
                    std::string error_message = "Failed to retrieve messages: " + error_info.message;
                    bot.log(dpp::ll_error, error_message);
                    response->reply("Failed to repopulate rules.");
                    return;
                }

                auto message_map = std::get<dpp::message_map>(confirmation.value);
                dpp::snowflake rules_message_id = 0;

                // Search for the rules message
                for (const auto& [id, msg] : message_map) {
                    if (msg.author.id == bot.me.id && !msg.embeds.empty() && msg.embeds[0].title == "Server Rules") {
                        rules_message_id = id;
                        bot.log(dpp::ll_info, "Found existing rules message with ID: " + std::to_string(rules_message_id));
                        break;
                    }
                }

                if (rules_message_id == 0) {
                    bot.log(dpp::ll_info, "No existing rules message found. Creating a new one.");
                    // Optionally, create a new rules message if not found
                    // This part is optional and depends on how you want to handle not finding an existing rules message
                }
                else {
                    // Create an embed with the updated rules from the database
                    dpp::embed rules_embed;
                    rules_embed.set_title("Server Rules");
                    rules_embed.set_color(0x0000FF); // Set color to blue
                    std::string rules_description;
                    for (size_t i = 0; i < rules.size(); ++i) {
                        rules_description += std::to_string(i + 1) + ". " + rules[i];
                        if (i < rules.size() - 1) { // Add a blank line between rules, except after the last one
                            rules_description += "\n\n";
                        }
                    }
                    rules_embed.set_description(rules_description);

                    // Edit the existing rules message with the new embed
                    dpp::message msg(channel_id, ""); // Empty string for content since we're using an embed
                    msg.id = rules_message_id;
                    msg.add_embed(rules_embed);

                    bot.message_edit(msg, [response](const dpp::confirmation_callback_t& edit_confirmation) {
                        if (edit_confirmation.is_error()) {
                            auto error_info = edit_confirmation.get_error();
                            std::string error_message = "Failed to repopulate rules: " + error_info.message;
                            response->reply("Failed to repopulate rules.");
                            return;
                        }
                        response->reply("Rules repopulated successfully.");
                        });
                }
                });
        }

        else if (sub_command == "button") {
            dpp::snowflake channel_id = std::get<dpp::snowflake>(event.get_parameter("channel"));
            std::string button_text = std::get<std::string>(event.get_parameter("button-text"));
            bot.log(dpp::ll_info, "Adding button to rules in channel: " + std::to_string(channel_id));

            auto response = std::make_shared<dpp::slashcommand_t>(event);

            // Retrieve messages from the channel to find the rules message
            bot.messages_get(channel_id, 0, 0, 0, 100, [response, &bot, channel_id, button_text](const dpp::confirmation_callback_t& confirmation) {
                if (confirmation.is_error()) {
                    auto error_info = confirmation.get_error();
                    std::string error_message = "Failed to retrieve messages: " + error_info.message;
                    bot.log(dpp::ll_error, error_message);
                    response->reply("Failed to add button to rules.");
                    return;
                }

                auto message_map = std::get<dpp::message_map>(confirmation.value);
                dpp::snowflake rules_message_id = 0;
                dpp::embed existing_embed;

                // Search for the rules message
                for (const auto& [id, msg] : message_map) {
                    if (msg.author.id == bot.me.id && !msg.embeds.empty() && msg.embeds[0].title == "Server Rules") {
                        rules_message_id = id;
                        existing_embed = msg.embeds[0]; // Retrieve the existing embed
                        break;
                    }
                }

                if (rules_message_id == 0) {
                    response->reply("No existing rules message found.");
                    return;
                }

                // Create a button component
                dpp::component button_component = dpp::component().set_type(dpp::cot_button).set_label(button_text).set_style(dpp::cos_primary).set_id("rules_button");

                // Create an action row and add the button component to it
                dpp::component action_row = dpp::component().set_type(dpp::cot_action_row).add_component(button_component);

                // Edit the message to include the existing embed and the action row with the button
                dpp::message msg = dpp::message(channel_id, "").add_embed(existing_embed).add_component(action_row);
                msg.id = rules_message_id;
                bot.message_edit(msg, [response, &bot](const dpp::confirmation_callback_t& edit_confirmation) {
                    if (edit_confirmation.is_error()) {
                        auto error_info = edit_confirmation.get_error();
                        std::string error_message = "Failed to add button to rules: " + error_info.message;
                        bot.log(dpp::ll_error, error_message);
                        response->reply("Failed to add button to rules.");
                    }
                    else {
                        response->reply("Button added to rules successfully.");
                    }
                    });
                });
                }

        else {
            // Unknown subcommand
            bot.log(dpp::ll_error, "Unknown subcommand: " + sub_command);
            event.reply("Unknown subcommand.");
        }
    }

} // namespace commands
