#include "FunCommand.h"

namespace commands {

    dpp::slashcommand register_fun_command(dpp::cluster& bot) {
        dpp::slashcommand fun_command("fun", "Fun commands", bot.me.id);
        fun_command.add_option(
            dpp::command_option(dpp::co_sub_command, "cat", "Get a random cat picture")
        );
        fun_command.add_option(
            dpp::command_option(dpp::co_sub_command, "hug", "Get a random hug GIF")
            .add_option(dpp::command_option(dpp::co_user, "user", "The user to hug", true))

        );
        return fun_command;
    }

    void handle_fun_command(const dpp::slashcommand_t& event, dpp::cluster& bot) {
        // Get the subcommand
        std::string sub_command = event.command.get_command_interaction().options[0].name;

        if (sub_command == "cat") {
            static bool use_cataas = true;  // Flag to alternate between APIs

            if (use_cataas) {
                // Use the Cataas API
                bot.request("https://cataas.com/cat", dpp::m_get, [event, &bot](const dpp::http_request_completion_t& response) {
                    if (response.status == 200) {
                        dpp::embed embed;
                        embed.set_color(dpp::colors::blurple);
                        embed.set_title("Cat!");
                        embed.set_image("attachment://cat.jpg");

                        event.reply(
                            dpp::message()
                            .add_embed(embed)
                            .add_file("cat.jpg", response.body)
                        );
                    }
                    else {
                        event.reply("Failed to retrieve a cat picture from Cataas.");
                    }
                    });
            }
            else {
                // Use The Cat API
                bot.request("https://api.thecatapi.com/v1/images/search", dpp::m_get, [event, &bot](const dpp::http_request_completion_t& response) {
                    if (response.status == 200) {
                        nlohmann::json result = nlohmann::json::parse(response.body);
                        std::string image_url = result[0]["url"];

                        dpp::embed embed;
                        embed.set_color(dpp::colors::blurple);
                        embed.set_title("Cat!");
                        embed.set_image(image_url);

                        event.reply(
                            dpp::message().add_embed(embed)
                        );
                    }
                    else {
                        event.reply("Failed to retrieve a cat picture from The Cat API.");
                    }
                    });
            }

            use_cataas = !use_cataas;  // Toggle the flag for the next command invocation
        }
        else if (sub_command == "hug") {
            // Get the mentioned user
            dpp::snowflake user_id = std::get<dpp::snowflake>(event.get_parameter("user"));
            dpp::user mentioned_user = bot.user_get_sync(user_id);

            // Get the user who invoked the command
            dpp::user invoker = event.command.usr;

            // Create the message text with user mentions
            std::string message_text = mentioned_user.get_mention() + " hugged by " + invoker.get_mention();

            // Use the nekos.best API for hugs
            bot.request("https://nekos.best/api/v2/hug", dpp::m_get, [event, &bot, message_text](const dpp::http_request_completion_t& response) {
                if (response.status == 200) {
                    nlohmann::json result = nlohmann::json::parse(response.body);
                    std::string gif_url = result["results"][0]["url"];
                    std::string anime_name = result["results"][0]["anime_name"];

                    dpp::embed embed;
                    embed.set_color(dpp::colors::blurple);
                    embed.set_title("From the Anime: " + anime_name);
                    embed.set_image(gif_url);

                    // Send the message text and embed in a single response
                    event.reply(
                        dpp::message(message_text).add_embed(embed)
                    );
                }
                else {
                    event.reply("Failed to retrieve a hug GIF.");
                }
                });
        }
    }
}
    


    
