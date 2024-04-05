#include "CreateRolesButton.h"
#include <sstream>

namespace commands {
    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::istringstream iss(str);
        std::string token;
        while (std::getline(iss, token, delimiter)) {
            // Trim leading and trailing whitespace from the token
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);
            tokens.push_back(token);
        }
        return tokens;
    }

    std::vector<dpp::snowflake> split_to_snowflakes(const std::string& str, char delimiter) {
        std::vector<dpp::snowflake> snowflakes;
        std::istringstream iss(str);
        std::string token;
        while (std::getline(iss, token, delimiter)) {
            try {
                snowflakes.push_back(std::stoull(token));
            }
            catch (const std::invalid_argument& e) {
                // Skip invalid snowflakes
            }
            catch (const std::out_of_range& e) {
                // Skip out-of-range snowflakes
            }
        }
        return snowflakes;
    }

    dpp::slashcommand register_createrolesbutton_command(dpp::cluster& bot) {
        dpp::slashcommand createrolesbutton_command("createrolesbutton", "Create buttons to assign roles", bot.me.id);
        createrolesbutton_command.add_option(dpp::command_option(dpp::co_string, "buttontexts", "The texts to display on the buttons (comma-separated)", true));
        createrolesbutton_command.add_option(dpp::command_option(dpp::co_string, "roleids", "The role IDs to assign (comma-separated)", true));
        createrolesbutton_command.add_option(dpp::command_option(dpp::co_channel, "channel", "The channel where the message is located", true));
        createrolesbutton_command.add_option(dpp::command_option(dpp::co_string, "messageid", "The ID of the message to add the buttons to", false));
        createrolesbutton_command.add_option(dpp::command_option(dpp::co_string, "buttonemojis", "The emojis to set on the buttons (comma-separated, optional)", false));
        createrolesbutton_command.add_option(dpp::command_option(dpp::co_string, "buttoncolors", "The colors of the buttons (comma-separated, optional)", false));
        createrolesbutton_command.add_option(dpp::command_option(dpp::co_string, "embedtitle", "The title of the embed (optional)", false));
        createrolesbutton_command.add_option(dpp::command_option(dpp::co_string, "embeddescription", "The description of the embed (use '\\n' for new lines, optional)", false));
        createrolesbutton_command.add_option(dpp::command_option(dpp::co_string, "embedcolor", "The color of the embed (optional)", false));
        createrolesbutton_command.add_option(dpp::command_option(dpp::co_string, "embedfooter", "The footer text of the embed (optional)", false));
        createrolesbutton_command.add_option(dpp::command_option(dpp::co_string, "embedthumbnailurl", "The URL of the thumbnail to include in the embed (optional)", false));
        
        createrolesbutton_command.set_default_permissions(dpp::p_manage_roles);

        return createrolesbutton_command;
    }

    void handle_createrolesbutton_command(const dpp::slashcommand_t& event, dpp::cluster& bot) {
        std::string button_texts_str = std::get<std::string>(event.get_parameter("buttontexts"));
        std::string role_ids_str = std::get<std::string>(event.get_parameter("roleids"));
        dpp::snowflake channel_id = std::get<dpp::snowflake>(event.get_parameter("channel"));

        std::vector<std::string> button_texts = split(button_texts_str, ',');
        std::vector<dpp::snowflake> role_ids = split_to_snowflakes(role_ids_str, ',');

        if (button_texts.size() != role_ids.size() || button_texts.size() > 10) {
            event.reply("Invalid number of button texts or role IDs. Maximum 10 buttons allowed.");
            return;
        }

        std::vector<std::string> button_emojis;
        dpp::command_value button_emojis_value = event.get_parameter("buttonemojis");
        if (std::holds_alternative<std::string>(button_emojis_value)) {
            std::string button_emojis_str = std::get<std::string>(button_emojis_value);
            button_emojis = split(button_emojis_str, ',');
            if (!button_emojis.empty() && button_emojis.size() != button_texts.size()) {
                event.reply("Number of button emojis doesn't match the number of buttons.");
                return;
            }
        }

        std::vector<std::string> button_colors;
        dpp::command_value button_colors_value = event.get_parameter("buttoncolors");
        if (std::holds_alternative<std::string>(button_colors_value)) {
            std::string button_colors_str = std::get<std::string>(button_colors_value);
            button_colors = split(button_colors_str, ',');
            if (!button_colors.empty() && button_colors.size() != button_texts.size()) {
                event.reply("Number of button colors doesn't match the number of buttons.");
                return;
            }
        }

        std::vector<dpp::component> action_rows;
        dpp::component current_action_row;

        for (size_t i = 0; i < button_texts.size(); ++i) {
            dpp::component button_component = dpp::component()
                .set_type(dpp::cot_button)
                .set_label(button_texts[i])
                .set_style(dpp::cos_primary)
                .set_id("role_button_" + std::to_string(role_ids[i]));

            if (i < button_emojis.size()) {
                button_component.set_emoji(button_emojis[i]);
            }

            if (i < button_colors.size()) {
                if (button_colors[i] == "primary") {
                    button_component.set_style(dpp::cos_primary);
                }
                else if (button_colors[i] == "secondary") {
                    button_component.set_style(dpp::cos_secondary);
                }
                else if (button_colors[i] == "success") {
                    button_component.set_style(dpp::cos_success);
                }
                else if (button_colors[i] == "danger") {
                    button_component.set_style(dpp::cos_danger);
                }
            }

            if (i % 5 == 0 && i > 0) {
                action_rows.push_back(current_action_row);
                current_action_row = dpp::component().set_type(dpp::cot_action_row);
            }

            current_action_row.add_component(button_component);
        }

        action_rows.push_back(current_action_row);

        dpp::embed embed;

        dpp::command_value embed_title_value = event.get_parameter("embedtitle");
        if (std::holds_alternative<std::string>(embed_title_value)) {
            std::string embed_title = std::get<std::string>(embed_title_value);
            if (!embed_title.empty()) {
                embed.set_title(embed_title);
            }
        }

        dpp::command_value embed_description_value = event.get_parameter("embeddescription");
        if (std::holds_alternative<std::string>(embed_description_value)) {
            std::string embed_description = std::get<std::string>(embed_description_value);
            if (!embed_description.empty()) {
                // Replace the new line placeholder with the actual new line character
                size_t pos = 0;
                while ((pos = embed_description.find("\\n", pos)) != std::string::npos) {
                    embed_description.replace(pos, 2, "\n");
                    pos++;
                }
                embed.set_description(embed_description);
            }
        }

        dpp::command_value embed_color_value = event.get_parameter("embedcolor");
        if (std::holds_alternative<std::string>(embed_color_value)) {
            std::string embed_color_str = std::get<std::string>(embed_color_value);
            if (!embed_color_str.empty()) {
                try {
                    uint32_t embed_color = std::stoul(embed_color_str, nullptr, 16);
                    embed.set_color(embed_color);
                }
                catch (const std::invalid_argument& e) {
                    event.reply("Invalid embed color provided. Please provide a valid hexadecimal color code.");
                    return;
                }
                catch (const std::out_of_range& e) {
                    event.reply("Embed color is out of range. Please provide a valid hexadecimal color code.");
                    return;
                }
            }
        }

        dpp::command_value embed_footer_value = event.get_parameter("embedfooter");
        if (std::holds_alternative<std::string>(embed_footer_value)) {
            std::string embed_footer_text = std::get<std::string>(embed_footer_value);
            if (!embed_footer_text.empty()) {
                dpp::embed_footer footer;
                footer.set_text(embed_footer_text);
                embed.set_footer(footer);
            }
        }
        // Add the current time as an additional field to the embed
        std::time_t current_time = std::time(nullptr);
        std::stringstream timestamp_stream;
        timestamp_stream << "<t:" << current_time << ":f>";
        std::string timestamp_field = timestamp_stream.str();

        embed.add_field("Created At", timestamp_field, true);

        dpp::command_value embed_thumbnail_url_value = event.get_parameter("embedthumbnailurl");
        if (std::holds_alternative<std::string>(embed_thumbnail_url_value)) {
            std::string embed_thumbnail_url = std::get<std::string>(embed_thumbnail_url_value);
            if (!embed_thumbnail_url.empty()) {
                embed.set_thumbnail(embed_thumbnail_url);
            }
        }

        if (!embed.title.empty() || !embed.description.empty() || embed.color.has_value() && embed.color.value() != 0 || (embed.footer.has_value() && !embed.footer.value().text.empty()) || (embed.thumbnail.has_value() && !embed.thumbnail.value().url.empty())) {
            dpp::message message = dpp::message().set_channel_id(channel_id);
            message.add_embed(embed);

            for (const auto& action_row : action_rows) {
                message.add_component(action_row);
            }

            bot.message_create(message, [event, &bot](const dpp::confirmation_callback_t& callback) {
                if (callback.is_error()) {
                    event.reply("Failed to create embed with roles buttons.");
                    bot.log(dpp::loglevel::ll_debug, "Error creating embed with roles buttons: " + callback.get_error().message);
                }
                else {
                    event.reply("Embed created with roles buttons successfully.");
                    bot.log(dpp::loglevel::ll_debug, "Embed created with roles buttons successfully.");
                }
                });

            return;
        }

        dpp::command_value message_id_value = event.get_parameter("messageid");
        if (std::holds_alternative<std::string>(message_id_value)) {
            std::string message_id_str = std::get<std::string>(message_id_value);
            if (!message_id_str.empty()) {
                try {
                    dpp::snowflake message_id = std::stoull(message_id_str);

                    // Retrieve the message using message_get()
                    bot.message_get(message_id, channel_id, [event, action_rows, &bot](const dpp::confirmation_callback_t& callback) {
                        if (callback.is_error()) {
                            event.reply("Failed to retrieve the message.");
                            return;
                        }

                        dpp::message message = std::get<dpp::message>(callback.value);

                        // Edit the retrieved message to include the new action rows with the buttons
                        for (const auto& action_row : action_rows) {
                            message.add_component(action_row);
                        }
                        bot.message_edit(message, [event](const dpp::confirmation_callback_t& callback) {
                            if (callback.is_error()) {
                                event.reply("Failed to add roles buttons to the message.");
                            }
                            else {
                                event.reply("Roles buttons added to the message successfully.");
                            }
                            });
                        });

                    return;
                }
                catch (const std::invalid_argument& e) {
                    event.reply("Invalid message ID provided.");
                    return;
                }
                catch (const std::out_of_range& e) {
                    event.reply("Message ID is out of range.");
                    return;
                }
            }
        }

        event.reply("Please provide either a valid message ID or embed details to create roles buttons.");
    }
}