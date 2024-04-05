#pragma once
#include <dpp/dpp.h>

namespace commands {
    dpp::slashcommand register_grantroles_command(dpp::cluster& bot);
    void handle_grantroles_command(const dpp::slashcommand_t& event, dpp::cluster& bot);
}
