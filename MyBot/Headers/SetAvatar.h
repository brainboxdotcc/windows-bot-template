#pragma once
#include <dpp/dpp.h>

namespace commands {
    dpp::slashcommand register_setavatar_command(dpp::cluster& bot);
    void handle_setavatar_command(const dpp::slashcommand_t& event, dpp::cluster& bot);
}
