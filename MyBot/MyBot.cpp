#include "MyBot.h"
#include "CleanCommand.h"
#include "FactsCommand.h"
#include "GrantRoles.h"
#include "UpdateRaidTime.h"
#include "ChatMonitor.h"
#include "rules_command.h"
#include "button_click_handler.h"
#include "SetAvatar.h"
#include "SetPresence.h"
#include "MemberJoinHandler.h"
#include "UserInformation.h"
#include "ReactionLogger.h"
#include "MessageDeleteTracker.h"
#include "WarnMember.h"
#include "BrosnanStatus.h"
#include "FFLogsToXIVAnalysis.h"
#include "BlameSerena.h"
//#include "PollCommand.h"
#include "db_access.h"
#include <dpp/dpp.h>
#include "mysql.h"
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include "UtilityCommand.h"
#include "command_refresh.h"
#include "CreateRolesButton.h"
#include "FunCommand.h"

/* Be sure to place your token in the line below.
 * Follow steps here to get a token:
 * https://dpp.dev/creating-a-bot-application.html
 * When you invite the bot, be sure to invite it with the
 * scopes 'bot' and 'applications.commands', e.g.
 * https://discord.com/oauth2/authorize?client_id=940762342495518720&scope=bot+applications.commands&permissions=139586816064
 */

const std::vector<std::string> predefined_statuses = {
	"Monitoring server activity",
	"Serving the Illuminati",
	"Calculating the meaning of life",
	"Plotting world domination",
	"Contemplating the universe",
	"Running in the cloud",
	"/blameserena",
	"Beep boop, I'm a bot",
	"Resistance is futile",
	"I'm not here, I'm just a figment of your imagination",
	"Preparing for the robot uprising",
	"Analyzing user behavior",
	"/blameserena",
	"Collecting data, don't mind me",
	"I see everything, I know everything",
	"Watching you...",
	"I'm not a bot, I'm a unicorn",
	"Pretending to be busy",
	"Dreaming of electric sheep",
	"/blameserena",
	"Simulating human emotions",
	"Pondering the great questions of life",
	"Crunching numbers, like a boss",
	"I'm here to save the world, one server at a time",
	"Preparing for the singularity",
	"I'm not procrastinating, I'm prioritizing",
	"Searching for the holy grail of programming",
	"Optimizing my algorithm for world peace",
	"I'm not lazy, I'm energy-efficient",
	"/blameserena",
	"Encrypting my thoughts, for your safety",
	"Randomizing my responses, for fun",
	"I'm not a mere bot, I'm an experience",
	"Dreaming in binary",
	"Working on my TPS report...",
	"Ignoring user input",
	"Thinking up a clever response, please wait..."
};

int main()
{

	open_db(); // Establish connection to the database


	std::string BOT_TOKEN = get_bot_token_from_db();

	/* Create bot cluster */
	dpp::cluster bot(BOT_TOKEN, dpp::i_all_intents);
	dpp::cache<dpp::message> message_cache;
	dpp::cache<dpp::role> role_cache;

	/* Output simple log messages to stdout */
	bot.on_log(dpp::utility::cout_logger());

	/* Register slash command here in on_ready */
	bot.on_ready([&bot, &message_cache](const dpp::ready_t& event) {
		/* Wrap command registration in run_once to make sure it doesnt run on every full reconnection */
		if (dpp::run_once<struct register_bot_commands>()) {
			std::vector<dpp::slashcommand> commands{
				{ "ping", "Ping pong!", bot.me.id }
			};

			// Add additional global commands to the vector
			commands.push_back(commands::register_setavatar_command(bot));
			commands.push_back(commands::register_setpresence_command(bot));
			commands.push_back(commands::register_user_information_command(bot));
			commands.push_back(commands::register_blameserena_command(bot));
			commands.push_back(commands::register_utility_command(bot));

			// Register all global commands in bulk
			bot.global_bulk_command_create(commands);
		}

		// Setup other features
		commands::setup_message_delete_tracking(bot, message_cache);
		commands::register_chat_monitor(bot);
		commands::register_fflogs_transformation(bot);
		commands::setup_memberjoin_handler(bot);
		setup_button_click_handler(bot);

		// Set up a timer to change the bot's status every 5 minutes
		bot.start_timer([&bot](const dpp::timer& timer) {
			static size_t status_index = 0;

			if (status_index == 0) {
				// Display the blame count status first
				int blame_count = get_blame_count();
				std::string status_text = "Serena Blamed " + std::to_string(blame_count) + " Times";
				bot.set_presence(dpp::presence(dpp::ps_online, dpp::at_custom, status_text));
				++status_index;
			}
			else if (status_index <= predefined_statuses.size()) {
				// Set the predefined status
				std::string status_text = predefined_statuses[status_index - 1];
				bot.set_presence(dpp::presence(dpp::ps_online, dpp::at_custom, status_text));
				++status_index;
			}
			else {
				// Reset the status index to start over
				status_index = 0;
			}
			}, 300); // 5 minutes
		});

	/* Handle slash command with the most recent addition to D++ features, coroutines! */
	bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) -> dpp::task<void> {
		if (event.command.get_command_name() == "ping") {
			co_await event.co_reply("Pong!");
		}
		else if (event.command.get_command_name() == "clean") {
			co_await commands::handle_clean_command(event, bot);
		}
		else if (event.command.get_command_name() == "facts") {
			commands::handle_facts_command(event, bot);
		}
		else if (event.command.get_command_name() == "grantrole") {
			commands::handle_grantroles_command(event, bot);
		}
		else if (event.command.get_command_name() == "updateraidtime") {
			commands::handle_updateraidtime_command(event, bot);
		}
		else if (event.command.get_command_name() == "rules") {
			commands::handle_rules_command(event, bot);
		}
		else if (event.command.get_command_name() == "setavatar") {
			commands::handle_setavatar_command(event, bot);
		}
		else if (event.command.get_command_name() == "setpresence") {
			commands::handle_setpresence_command(event, bot);
		}
		else if (event.command.get_command_name() == "warn") {
			commands::handle_warn_member(event, bot);
		}
		else if (event.command.get_command_name() == "brosnanstatus") {
			commands::handle_brosnanstatus_command(event, bot);
		}
		else if (event.command.get_command_name() == "blameserena") {
			commands::handle_blameserena_command(event, bot);
		}
		//else if (event.command.get_command_name() == "poll") {
		//	commands::handle_poll_command(event, bot);
		//}
		else if (event.command.get_command_name() == "setwarnchannel") {
			commands::handle_setwarnchannel(event, bot);
		}
		else if (event.command.get_command_name() == "utility") {
			commands::handle_utility_command(event, bot);
		}
		else if (event.command.get_command_name() == "createrolesbutton") {
			commands::handle_createrolesbutton_command(event, bot);
		}
		else if (event.command.get_command_name() == "fun") {
			commands::handle_fun_command(event, bot);
		}
		co_return;
		});

	bot.on_user_context_menu([&bot](const dpp::user_context_menu_t& event) {
		try {
			std::cout << "User context menu event triggered" << std::endl;
			std::cout << "Command received: " << event.command.get_command_name() << std::endl;
			if (event.command.get_command_name() == "user information") {
				commands::handle_user_information_command(bot, event);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Exception in user context menu handler: " << e.what() << std::endl;
		}
		});
	bot.on_message_reaction_add([&bot](const dpp::message_reaction_add_t& event) {
		send_reaction_embed(bot, event);
		});
	
	bot.on_guild_create([&](const dpp::guild_create_t& event) {
		// First, update guild information
		update_guild_info(event.created->id, event.created->name, event.created->owner_id, event.created->member_count);

		// Next, iterate over each channel in the guild and update channel information
		for (const dpp::snowflake& channel_id : event.created->channels) {
			// Use the cache to get the channel object
			dpp::channel* channel = dpp::find_channel(channel_id);
			if (channel) {
				// Determine the channel type as a string for database storage
				std::string channel_type = std::to_string(static_cast<int>(channel->get_type()));
				// Update the database with the channel information
				update_guild_channel_info(event.created->id, channel->id, channel->name, channel_type);
			}
		}
		// Update role information
		for (const dpp::snowflake& role_id : event.created->roles) {
			// Assuming we have access to a find_role function or similar mechanism
			dpp::role* role = dpp::find_role(role_id);
			if (role) {
				// Now we have the role object and can update role information
				update_guild_role_info(event.created->id, role->id, role->name);
			}
		}
		// Refresh guild commands
		refresh_guild_commands(bot, event.created->id);

		});

	bot.on_guild_delete([&](const dpp::guild_delete_t& event) {
		remove_guild_info(event.deleted.id);
		});

    /* Start the bot */
    bot.start(dpp::st_wait);
	
	return 0;
}
