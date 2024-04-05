#pragma once
#include <dpp/dpp.h>

namespace commands {
	dpp::slashcommand register_warn_member_command(dpp::cluster& bot);
	dpp::slashcommand register_setwarnchannel_command(dpp::cluster& bot);
	void handle_warn_member(const dpp::slashcommand_t& event, dpp::cluster& bot);
	void handle_setwarnchannel(const dpp::slashcommand_t& event, dpp::cluster& bot);
}