#include "BrosnanStatus.h"
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <iostream> // For std::cerr
#include <regex> // For std::regex

namespace commands {

    dpp::slashcommand register_brosnanstatus_command(dpp::cluster& bot) {
        dpp::slashcommand brosnanstatus_command("brosnanstatus", "Check if Pierce Brosnan is alive or has passed away", bot.me.id);
        return brosnanstatus_command;
    }

    void handle_brosnanstatus_command(const dpp::slashcommand_t& event, dpp::cluster& bot) {
        std::string api_url = "https://api.wikimedia.org/core/v1/wikipedia/en/page/Pierce_Brosnan/html";

        // Make the request without additional headers
        bot.request(api_url, dpp::http_method::m_get, [event, &bot](const dpp::http_request_completion_t& http) {
            if (http.status == 200) {
                std::string content = http.body;
                std::string status_message;

                // Using a regex to search for the "Died" text within the HTML content
                std::regex died_regex(R"(<th[^>]*>Died<\/th>)");
                bool has_died = std::regex_search(content, died_regex);

                if (has_died) {
                    status_message = "Pierce Brosnan has passed away.";
                }
                else {
                    status_message = "Pierce Brosnan is alive.";
                }

                dpp::embed brosnan_status_embed;
                brosnan_status_embed.set_title("Pierce Brosnan Status")
                    .set_description(status_message)
                    .set_color(has_died ? dpp::colors::red : dpp::colors::green);

                dpp::message reply_message;
                reply_message.add_embed(brosnan_status_embed);

                event.reply(reply_message);
            }
            else {
                event.reply("Failed to retrieve information for Pierce Brosnan.");
            }
            });
    }
}
