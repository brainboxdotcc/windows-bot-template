#include <dpp/dpp.h>
#include <iostream>

int main()
{
    try {
        // Be sure to place your token in the line below, and uncomment the line
        // Follow steps here to get a token: https://dpp.dev/creating-a-bot-application.html
        //dpp::cluster bot("place token here");

        bot.on_ready([&bot](const dpp::ready_t& event) {
            bot.log(dpp::ll_info, "Logged in as " + bot.me.username);
        });

        bot.on_message_create([&bot](const dpp::message_create_t& event) {
            if (event.msg.content == "!ping") {
                bot.message_create(dpp::message(event.msg.channel_id, "Pong!"));
            }
        });

        bot.on_log([](const dpp::log_t& event) {
            if (event.severity > dpp::ll_trace) {
                std::cout << dpp::utility::loglevel(event.severity) << ": " << event.message << "\n";
            }
        });

        bot.start(false);
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }

    return 0;
}
