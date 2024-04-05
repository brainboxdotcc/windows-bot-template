#include <dpp/dpp.h>

void send_reaction_embed(dpp::cluster& bot, const dpp::message_reaction_add_t& event) {
    // Define the target guild ID to monitor
    dpp::snowflake target_guild_id = 891873251976949770;

    // Check if the reaction occurred in the target guild
    if (event.reacting_guild == nullptr || event.reacting_guild->id != target_guild_id) {
        // Reaction is not in the target guild, ignore it
        return;
    }
    if (event.reacting_user.is_bot()) {
        // Ignore reactions from bots
        return;
    }

    dpp::snowflake user_id = event.reacting_user.id;
    std::string emoji_name = event.reacting_emoji.name;

    // Assuming the event contains valid guild_id and channel_id
    std::string message_link = dpp::utility::message_url(event.reacting_guild->id, event.channel_id, event.message_id);

    // Attempt to retrieve the nickname of the reacting user from the reacting_member
    std::string nickname_or_username = event.reacting_member.get_nickname().empty() ? event.reacting_user.username : event.reacting_member.get_nickname();

    // Create the embed with the user's nickname in the title
    dpp::embed reaction_embed;
    reaction_embed.set_title("Reaction Received from " + nickname_or_username)
        .set_color(0x00ff00)
        .add_field("User Reacted", "<@" + std::to_string(user_id) + ">", true)
        .add_field("Emoji Used", emoji_name, true)
        .add_field("Message Link", message_link, false); // Add the message link to the embed

    // Channel ID to send the embed to
    dpp::snowflake channel_id = 1150994279376900207;

    // Send the embed to the specified channel
    bot.message_create(dpp::message(channel_id, "").add_embed(reaction_embed), [](const dpp::confirmation_callback_t& response) {
        if (response.is_error()) {
            std::cerr << "Error sending embed: " << response.get_error().message << "\n";
        }
        else {
            std::cout << "Embed sent successfully\n";
        }
        });
}
