#pragma once
#include <dpp/dpp.h>

namespace commands {
    dpp::slashcommand register_facts_command(dpp::cluster& bot);
    void handle_facts_command(const dpp::slashcommand_t& event, dpp::cluster& bot);
}
