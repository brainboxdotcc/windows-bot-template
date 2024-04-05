#pragma once
#include <dpp/dpp.h>

namespace commands {
    dpp::slashcommand register_poll_command(dpp::cluster& bot);
    void handle_poll_command(const dpp::slashcommand_t& event, dpp::cluster& bot);
}