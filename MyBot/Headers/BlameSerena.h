#pragma once
#include <dpp/dpp.h>

namespace commands {
    dpp::slashcommand register_blameserena_command(dpp::cluster& bot);
    void handle_blameserena_command(const dpp::slashcommand_t& event, dpp::cluster& bot);

}