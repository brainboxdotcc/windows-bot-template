#pragma once

#include <dpp/dpp.h>

namespace commands {
    dpp::slashcommand register_createrolesbutton_command(dpp::cluster& bot);
    void handle_createrolesbutton_command(const dpp::slashcommand_t& event, dpp::cluster& bot);
}