#pragma once
#include <dpp/dpp.h>

namespace commands {
    dpp::slashcommand register_user_information_command(dpp::cluster& bot);
    void handle_user_information_command(dpp::cluster& bot, const dpp::user_context_menu_t& event);
}
