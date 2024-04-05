#pragma once
#include <dpp/dpp.h>

namespace commands {
    dpp::slashcommand register_fun_command(dpp::cluster& bot);
    void handle_fun_command(const dpp::slashcommand_t& event, dpp::cluster& bot);
}