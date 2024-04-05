#include "FFLogsToXIVAnalysis.h"
#include <regex>
#include <string>

namespace commands {

    void register_fflogs_transformation(dpp::cluster& bot) {
        bot.on_message_create([&bot](const dpp::message_create_t& event) {
            // Regular expression to match FFLogs report links
            std::regex fflogs_regex(R"(https:\/\/www\.fflogs\.com\/reports\/([a-zA-Z0-9]+))");
            std::smatch matches;

            std::string message_content = event.msg.content;
            if (std::regex_search(message_content, matches, fflogs_regex) && matches.size() > 1) {
                // Construct the new XIVAnalysis link
                std::string report_id = matches[1].str();
                std::string xiv_analysis_link = "https://xivanalysis.com/fflogs/" + report_id;

                // Post the new link to the same channel
                dpp::message new_message(event.msg.channel_id, xiv_analysis_link);
                bot.message_create(new_message);
            }
            });
    }

}