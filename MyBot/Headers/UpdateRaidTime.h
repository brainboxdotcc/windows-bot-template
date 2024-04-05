#pragma once
#include <dpp/dpp.h>

namespace commands {

	dpp::slashcommand register_updateraidtime_command(dpp::cluster& bot);
void handle_updateraidtime_command(const dpp::slashcommand_t& event, dpp::cluster& bot);

}
