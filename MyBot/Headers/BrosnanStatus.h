#pragma once
#include <dpp/dpp.h>

namespace commands {
    dpp::slashcommand register_brosnanstatus_command(dpp::cluster& bot);
    void handle_brosnanstatus_command(const dpp::slashcommand_t& event, dpp::cluster& bot);
}
