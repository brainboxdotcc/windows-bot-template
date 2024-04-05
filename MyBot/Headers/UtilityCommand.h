#pragma once
#include <dpp/dpp.h>

namespace commands {
	dpp::slashcommand register_utility_command(dpp::cluster& bot);
	void handle_utility_command(const dpp::slashcommand_t& event, dpp::cluster& bot);
}