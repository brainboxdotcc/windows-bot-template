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
    /* Create bot cluster */
    dpp::cluster bot(BOT_TOKEN);

    /* Handle slash command */
    bot.on_interaction_create([](const dpp::interaction_create_t& event) {
         if (event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        }
    });

    /* Register slash command here in on_ready */
    bot.on_ready([&bot](const dpp::ready_t& event) {
        /* Wrap command registration in run_once to make sure it doesnt run on every full reconnection */
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.guild_command_create(dpp::slashcommand("Ping", "Ping pong!", bot.me.id), MY_GUILD_ID);
        }
    });

    /* Output simple log messages to stdout */
    bot.on_log(dpp::utility::cout_logger());

    /* Start the bot */
    bot.start(false);

    return 0;
}
