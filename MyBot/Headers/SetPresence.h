#pragma once
#include <dpp/dpp.h>

namespace commands {
    dpp::slashcommand register_setpresence_command(dpp::cluster& bot);
    void handle_setpresence_command(const dpp::slashcommand_t& event, dpp::cluster& bot);
}
