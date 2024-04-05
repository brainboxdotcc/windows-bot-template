//#include "PollCommand.h"
//#include "db_access.h"
//
//namespace commands {
//
//	dpp::slashcommand register_poll_command(dpp::cluster& bot) {
//		dpp::slashcommand poll_command("poll", "Create or end a poll.", bot.me.id);
//		poll_command.add_option(
//			dpp::command_option(dpp::co_sub_command, "create", "Create a poll.")
//			.add_option(dpp::command_option(dpp::co_string, "question", "The poll question", true))
//			.add_option(dpp::command_option(dpp::co_string, "option1", "The first option", true))
//			.add_option(dpp::command_option(dpp::co_string, "option2", "The second option", true))
//			.add_option(dpp::command_option(dpp::co_string, "option3", "The third option", false))
//			.add_option(dpp::command_option(dpp::co_string, "option4", "The fourth option", false))
//			.add_option(dpp::command_option(dpp::co_string, "option5", "The fifth option", false))
//			.add_option(dpp::command_option(dpp::co_string, "option6", "The sixth option", false))
//			.add_option(dpp::command_option(dpp::co_string, "option7", "The seventh option", false))
//			.add_option(dpp::command_option(dpp::co_string, "option8", "The eighth option", false))
//		);
//		poll_command.add_option(
//			dpp::command_option(dpp::co_sub_command, "end", "End a poll.")
//		);
//		// Return the command object
//		return poll_command;
//	}
//
//	void handle_poll_command(const dpp::slashcommand_t& event, dpp::cluster& bot) {
//		std::string sub_command = event.command.get_command_interaction().options[0].name;
//		bot.log(dpp::ll_info, "Received poll command with subcommand: " + sub_command);
//		event.thinking();
//
//		if (sub_command == "create") {
//			std::string question = std::get<std::string>(event.get_parameter("question"));
//			std::string option1 = std::get<std::string>(event.get_parameter("option1"));
//			std::string option2 = std::get<std::string>(event.get_parameter("option2"));
//			std::string option3 = "";
//			std::string option4 = "";
//			std::string option5 = "";
//			std::string option6 = "";
//			std::string option7 = "";
//			std::string option8 = "";
//			if (event.get_parameter("option3").index() != 0) {
//				option3 = std::get<std::string>(event.get_parameter("option3"));
//			}
//			if (event.get_parameter("option4").index() != 0) {
//				option4 = std::get<std::string>(event.get_parameter("option4"));
//			}
//			if (event.get_parameter("option5").index() != 0) {
//				option5 = std::get<std::string>(event.get_parameter("option5"));
//			}
//			if (event.get_parameter("option6").index() != 0) {
//				option6 = std::get<std::string>(event.get_parameter("option6"));
//			}
//			if (event.get_parameter("option7").index() != 0) {
//				option7 = std::get<std::string>(event.get_parameter("option7"));
//			}
//			if (event.get_parameter("option8").index() != 0) {
//				option8 = std::get<std::string>(event.get_parameter("option8"));
//			}
//			std::cout << "Options extracted.\n"; // Log options extraction
//
//			// Construct the embed
//			dpp::embed pollEmbed;
//			pollEmbed.set_title("Poll: " + question);
//			pollEmbed.set_thumbnail("https://i.imgur.com/V9y9sPa.png"); // Replace with your own image URL
//			std::vector<std::string> options = { option1, option2, option3, option4, option5, option6, option7, option8 };
//			std::string emojis[] = { "\U00000031\U0000FE0F\U000020E3","\U00000032\U0000FE0F\U000020E3","\U00000033\U0000FE0F\U000020E3","\U00000034\U0000FE0F\U000020E3","\U00000035\U0000FE0F\U000020E3","\U00000036\U0000FE0F\U000020E3","\U00000037\U0000FE0F\U000020E3","\U00000038\U0000FE0F\U000020E3" };
//			for (size_t i = 0; i < options.size(); ++i) {
//				if (!options[i].empty()) {
//					pollEmbed.add_field(emojis[i] + " " + options[i], "", false);
//				}
//			}
//			std::cout << "Embed constructed.\n"; // Log embed construction
//
//			dpp::message msg(event.command.channel_id, "");
//			// Add the embed directly to the message
//			msg.embeds.push_back(pollEmbed);
//			std::cout << "Message prepared with embed.\n"; // Log message preparation
//
//			std::cout << "Number of options: " << options.size() << "\n";
//			for (const auto& opt : options) {
//				std::cout << "Option: " << opt << "\n";
//			}
//
//			// Send the message
//			bot.message_create(msg, [options, emojis, question, &bot, event](const dpp::confirmation_callback_t& callback) mutable {
//				if (callback.is_error()) {
//					bot.log(dpp::loglevel::ll_debug, "Error sending message: " + callback.get_error().message);
//					event.edit_original_response(dpp::message("Failed to create poll. Please try again."));
//					return;
//				}
//				auto sent_msg = std::get<dpp::message>(callback.value);
//				std::string sql = "REPLACE INTO active_polls (guild_id, message_id, channel_id, poll_title) VALUES (?, ?, ?, ?);";
//				std::vector<std::string> params = {
//					std::to_string(event.command.guild_id),
//					std::to_string(sent_msg.id),
//					std::to_string(sent_msg.channel_id),
//					"Poll: " + question // Or however you construct your poll title
//				};
//				execute_sql_with_params(sql, params);
//
//				bot.log(dpp::loglevel::ll_debug, "Message sent. Message ID: " + std::to_string(sent_msg.id));
//
//				// Loop to add reactions using the message object directly
//				for (size_t i = 0; i < options.size(); ++i) {
//					if (!options[i].empty()) {
//						std::string emoji = emojis[i];
//						bot.message_add_reaction(sent_msg, emoji, [i, emoji, &bot](const dpp::confirmation_callback_t& cb) {
//							if (cb.is_error()) {
//								bot.log(dpp::loglevel::ll_debug, "Error adding reaction for option " + std::to_string(i + 1) + ": " + emoji + ". Error: " + cb.get_error().message);
//							}
//							else {
//								bot.log(dpp::loglevel::ll_debug, "Successfully added reaction for option " + std::to_string(i + 1) + ": " + emoji);
//							}
//							});
//					}
//				}
//				event.edit_original_response(dpp::message("Poll created successfully!"));
//				});
//		}
//		if (sub_command == "end") {
//			dpp::snowflake guild_id = event.command.guild_id;
//
//			// Fetch active poll info from the database
//			auto poll_info = get_poll_info(guild_id);
//			if (!poll_info) {
//				event.edit_original_response(dpp::message("No active poll found for this guild."));
//				return;
//			}
//
//			dpp::snowflake message_id = std::get<0>(*poll_info);
//			dpp::snowflake channel_id = std::get<1>(*poll_info);
//			std::vector<std::string> emojis = {
//			"\U00000031\U0000FE0F\U000020E3", "\U00000032\U0000FE0F\U000020E3",
//			"\U00000033\U0000FE0F\U000020E3", "\U00000034\U0000FE0F\U000020E3",
//			"\U00000035\U0000FE0F\U000020E3", "\U00000036\U0000FE0F\U000020E3",
//			"\U00000037\U0000FE0F\U000020E3", "\U00000038\U0000FE0F\U000020E3" };
//			// Fetch the message for the active poll
//			bot.message_get(message_id, channel_id, [&bot, event, emojis, guild_id, message_id](const dpp::confirmation_callback_t& callback) {
//				if (callback.is_error()) {
//					event.edit_original_response(dpp::message("Failed to fetch the poll message."));
//					return;
//				}
//
//				auto fetched_msg = std::get<dpp::message>(callback.value);
//				if (fetched_msg.reactions.empty() || fetched_msg.embeds.empty()) {
//					event.edit_original_response(dpp::message("No votes found in the poll."));
//					return;
//				}
//
//				// Assuming each reaction corresponds to an embed field in order.
//				std::vector<int> vote_counts(fetched_msg.embeds[0].fields.size(), 0);
//
//				for (const auto& reaction : fetched_msg.reactions) {
//					int option_index = -1; // Initialize as not found
//					for (size_t i = 0; i < emojis.size(); i++) {
//						if (reaction.emoji_name == emojis[i]) { // Direct comparison of Unicode representations
//							option_index = i;
//							break;
//						}
//					}
//
//					if (option_index >= 0 && option_index < vote_counts.size()) {
//						vote_counts[option_index] += reaction.count - 1; // Adjust for bot's reaction if present
//					}
//				}
//
//				// Determine the result, handling ties
//				int max_votes = *std::max_element(vote_counts.begin(), vote_counts.end());
//				std::vector<std::string> winners;
//				for (size_t i = 0; i < vote_counts.size(); ++i) {
//					if (vote_counts[i] == max_votes) {
//						winners.push_back(fetched_msg.embeds[0].fields[i].name);
//					}
//				}
//
//				std::string response;
//				if (winners.empty()) {
//					response = "No votes were cast in the poll.";
//				}
//				else if (winners.size() == 1) {
//					response = "The winning option is: " + winners.front() + " with " + std::to_string(max_votes) + " votes.";
//				}
//				else {
//					response = "There's a tie between the following options: ";
//					for (const auto& winner : winners) {
//						if (winner != winners.front()) response += ", ";
//						response += winner;
//					}
//					response += " with " + std::to_string(max_votes) + " votes each.";
//				}
//
//				// Reply with the results
//				event.edit_original_response(response);
//
//				// Optionally, update the database to mark the poll as ended
//				remove_poll_info(guild_id);
//				});
//		}
//
//
//
//
//	}
//				
//}