#pragma once
#include <dpp/dpp.h>

namespace commands {

	dpp::slashcommand register_rules_command(dpp::cluster& bot);
	void handle_rules_command(const dpp::slashcommand_t& event, dpp::cluster& bot);

} // namespace commands
