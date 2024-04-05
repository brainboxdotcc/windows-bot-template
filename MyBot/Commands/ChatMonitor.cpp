#include "ChatMonitor.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <set>
#include <random>
#include <chrono>
#include "db_access.h"

// Initialize a random number generator
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_int_distribution<int> distribution(1, 100);

namespace commands {

    void get_emoji_from_openai(dpp::cluster& bot, const std::string& message, const dpp::message& original_message) {
        // OpenAI API setup
        const std::string endpoint = "https://api.openai.com/v1/chat/completions";
        const std::string api_key = get_openai_api_key(); // Replace with your actual OpenAI API key

        // Prepare the JSON payload
        nlohmann::json request_body = {
            {"model", "gpt-3.5-turbo"},
            {"temperature", 0.7},
            {"max_tokens", 256},
            {"top_p", 1},
            {"frequency_penalty", 0},
            {"presence_penalty", 0},
            {"messages", {
                {{"role", "system"}, {"content", "You are only capable of responding to the message with a single emoji that best represents the message."}},
                {{"role", "user"}, {"content", message}}
            }}
        };


        // Setup the headers
        std::multimap<std::string, std::string> headers = {
            {"Authorization", "Bearer " + api_key},
            {"Content-Type", "application/json"}
        };

        // Making the POST request to the OpenAI API
        std::cout << "Sending request to OpenAI API..." << std::endl;
        bot.request(endpoint, dpp::http_method::m_post,
            [&bot, original_message](const dpp::http_request_completion_t& response) {
                if (response.status == 200) { // HTTP OK
                    std::cout << "Received response from OpenAI API. Status: " << response.status << std::endl;
                    nlohmann::json response_json = nlohmann::json::parse(response.body);
                    std::cout << "OpenAI API Response: " << response_json.dump(4) << std::endl;
                    std::string emoji = response_json["choices"][0]["message"]["content"].get<std::string>();
                    bot.message_add_reaction(original_message, emoji); // React to the original message with the emoji
                }
                else {
                    std::cerr << "Error from OpenAI API. Status: " << response.status << ". Body: " << response.body << std::endl;
                }
            },
            request_body.dump(), "application/json", headers
        );
    }

    void register_chat_monitor(dpp::cluster& bot) {

        std::cout << "Random number generator seeded with: " << seed << std::endl;

        bot.on_message_create([&bot](const dpp::message_create_t& event) {
            if (event.msg.author.id == bot.me.id || event.msg.author.is_bot()) {
                return;
            }

            std::vector<std::string> keywords = { "pierce brosnan", "yoshi p", "yoship", "yoshi-p", "japan", "raid", "shion", "saskia", "erik", "amia", "opal", "lief", "dyna", "bot", "cat", "broken", "lol", "lmao", "a8s" };
            std::string message_content = event.msg.content;
            std::transform(message_content.begin(), message_content.end(), message_content.begin(), ::tolower);

            std::istringstream iss(message_content);
            std::set<std::string> tokens{ std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{} };

            bool trigger_word_found = false;

            for (const auto& keyword : keywords) {
                std::string lowercase_keyword = keyword;
                std::transform(lowercase_keyword.begin(), lowercase_keyword.end(), lowercase_keyword.begin(), ::tolower);

                if ((lowercase_keyword.find(' ') != std::string::npos && message_content.find(lowercase_keyword) != std::string::npos) ||
                    (tokens.find(lowercase_keyword) != tokens.end())) {

                    trigger_word_found = true;

                    int random_value = distribution(generator);

                    if (random_value <= 40) {
                        if (lowercase_keyword == "cat") {
                            bot.message_add_reaction(event.msg, "\U0001F431"); // Cat emoji
                        }
                        else if (lowercase_keyword == "broken") {
                            bot.message_add_reaction(event.msg, "\U0001F914"); // Thinking emoji
                        }
                        else if (lowercase_keyword == "lol" || lowercase_keyword == "lmao") {
                            bot.message_add_reaction(event.msg, "\U0001F923"); // ROFL emoji
                        }
                        else if (lowercase_keyword == "saskia") {
                            bot.message_add_reaction(event.msg, "\U0001F49C"); // Purple Heart emoji
                        }
                        else if (lowercase_keyword == "yoship" || lowercase_keyword == "yoshi p" || lowercase_keyword == "yoshi-p") {
                            bot.message_add_reaction(event.msg, "\:wine31:1223785508702781460"); // Yoshi-P emoji
                        }
                        else {
                            bot.message_add_reaction(event.msg, "\U0001F916"); // Robot emoji
                        }
                    }
                }
            }

            if (!trigger_word_found) {
                int random_value = distribution(generator);

                if (random_value <= 40) {
                    std::cout << "Triggering OpenAI API for message: " << message_content << std::endl;
                    get_emoji_from_openai(bot, message_content, event.msg);
                }
            }
            });
    }
}