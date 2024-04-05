#pragma once
#include <dpp/dpp.h>


namespace commands {

        void setup_memberjoin_handler(dpp::cluster& bot);
        dpp::slashcommand register_setmemberjoinchannel_command(dpp::cluster& bot);
}