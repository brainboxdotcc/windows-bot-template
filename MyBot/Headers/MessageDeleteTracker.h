#pragma once

namespace commands {

void setup_message_delete_tracking(dpp::cluster& bot, dpp::cache<dpp::message>& message_cache);
dpp::slashcommand register_setdeletechannel_command(dpp::cluster& bot);

}