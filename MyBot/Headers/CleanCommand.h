#pragma once
#include <dpp/dpp.h>

namespace commands {
    dpp::slashcommand register_clean_command(dpp::cluster& bot);
    auto handle_clean_command(const dpp::slashcommand_t& event, dpp::cluster& bot) -> dpp::task<void>;
}
