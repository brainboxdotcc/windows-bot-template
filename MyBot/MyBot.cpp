#include <dpp/dpp.h>
#include <iostream>

int main()
{
    dpp::cluster bot("put your bot token here");

    bot.on_ready([&bot](const dpp::ready_t& event) {
        bot.log(dpp::ll_info, "Logged in as " + bot.me.username);
    });

    bot.on_message_create([&bot](const dpp::message_create_t& event) {
        if (event.msg->content == "!ping") {
            bot.message_create(dpp::message(event.msg->channel_id, "Pong!"));
        }
    });

    bot.on_log([](const dpp::log_t& event) {
        std::cout << event.message << "\n";
    });

    bot.start(false); 

    return 0;
}
