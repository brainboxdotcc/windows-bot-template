#include <dpp/dpp.h>
#include <iostream>

/* Be sure to place your token in the line below.
 * Follow steps here to get a token: https://dpp.dev/creating-a-bot-application.html
 * You will also need to fill in your guild id. When you invite the bot, be sure to
 * invite it with the scopes 'bot' and 'applications.commands', e.g.
 * https://discord.com/oauth2/authorize?client_id=940762342495518720&scope=bot+applications.commands&permissions=139586816064
 */
const std::string    BOT_TOKEN    = "add your token here";
const dpp::snowflake MY_GUILD_ID  =  825407338755653642;

int main()
{
    try {
        /* Create bot cluster */
        dpp::cluster bot(BOT_TOKEN);

        /* The interaction create event is fired when someone issues your commands */
        bot.on_interaction_create([&bot](const dpp::interaction_create_t& event) {
            dpp::command_interaction cmd_data = std::get<dpp::command_interaction>(event.command.data);
            if (cmd_data.name == "ping") {
                /* Reply to ping command */
                event.reply(dpp::ir_channel_message_with_source, "Pong!");
            }
        });

        bot.on_ready([&bot](const dpp::ready_t& event) {
            /* Register the command */
            bot.guild_command_create(
                dpp::slashcommand()
                .set_name("ping")
                .set_description("Ping pong!")
                .set_application_id(bot.me.id),
                MY_GUILD_ID
            );

        });

        /* Simple logger */
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
