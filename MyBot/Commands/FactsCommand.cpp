#include "FactsCommand.h"
#include "pierce_brosnan_facts.h"
#include <dpp/nlohmann/json.hpp>
#include <dpp/httpsclient.h>
#include <random>
#include <vector>


namespace commands {

    dpp::slashcommand register_facts_command(dpp::cluster& bot) {
        dpp::slashcommand facts_command("facts", "Get interesting facts!", bot.me.id);
        facts_command.add_option(
            dpp::command_option(dpp::co_sub_command, "cat", "Get a random cat fact!")
        );
        facts_command.add_option(
			dpp::command_option(dpp::co_sub_command, "dog", "Get a random dog fact!")
		);
        facts_command.add_option(
            dpp::command_option(dpp::co_sub_command, "chucknorris", "Get a random Chuck Norris fact!")
        );
        facts_command.add_option(
			dpp::command_option(dpp::co_sub_command, "number", "Get a random number fact!")
		);
        facts_command.add_option(
			dpp::command_option(dpp::co_sub_command, "useless", "Get a random useless fact!")
		);
        facts_command.add_option(
			dpp::command_option(dpp::co_sub_command, "piercebrosnan", "Get a random Pierce Brosnan fact!")
        );

        // Return the command object
        return facts_command;
    }
    // Function to handle the response from the cat fact API
    void handle_cat_fact_response(const dpp::http_request_completion_t& http, dpp::cluster& bot, const dpp::slashcommand_t& event) {
        try {
            dpp::embed embed;
            std::string title, description;

            if (http.status == 200) {
                dpp::json json = dpp::json::parse(http.body);
                title = "Random Cat Fact";
                description = json["fact"].get<std::string>();

                embed.set_title(title)
                    .set_description(description)
                    .set_color(0x0099ff);
            }
            else {
                title = "Error";
                description = "Failed to fetch a cat fact.";

                embed.set_title(title)
                    .set_description(description)
                    .set_color(0xff0000);
            }

            // Reply to the event with the embed
            event.reply(dpp::message().add_embed(embed), [event](const dpp::confirmation_callback_t& callback) {
                if (callback.is_error()) {
                    std::cerr << "Error sending reply: " << callback.get_error().message << std::endl;
                }
                else {
                    std::cerr << "Reply sent successfully." << std::endl;
                }
                }); //<-- still tells me in the IDE that a breakpoint execution was executed here
        }

        catch (const std::exception& e) {
            // Log the exception message
            std::cerr << "Exception in handle_cat_fact_response: " << e.what() << std::endl;
        }
    }
    // Function to handle the response from the dog fact API
    void handle_dog_fact_response(const dpp::http_request_completion_t& http, dpp::cluster& bot, const dpp::slashcommand_t& event) {
        try {
            dpp::embed embed;
            std::string title, description;

            if (http.status == 200) {
                dpp::json json = dpp::json::parse(http.body);
                title = "Random Dog Fact";
                description = json["facts"][0].get<std::string>();

                embed.set_title(title)
                    .set_description(description)
                    .set_color(0x0099ff);
            }
            else {
                title = "Error";
                description = "Failed to fetch a dog fact.";

                embed.set_title(title)
                    .set_description(description)
                    .set_color(0xff0000);
            }

            event.reply(dpp::message().add_embed(embed), [event](const dpp::confirmation_callback_t& callback) {
                if (callback.is_error()) {
                    std::cerr << "Error sending reply: " << callback.get_error().message << std::endl;
                }
                else {
                    std::cerr << "Reply sent successfully." << std::endl;
                }
                });
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in handle_dog_fact_response: " << e.what() << std::endl;
        }
    }
    // Function to handle the response from the Chuck Norris fact API
    void handle_chuck_norris_fact_response(const dpp::http_request_completion_t& http, dpp::cluster& bot, const dpp::slashcommand_t& event) {
        try {
			dpp::embed embed;
			std::string title, description;

            if (http.status == 200) {
				dpp::json json = dpp::json::parse(http.body);
				title = "Random Chuck Norris Fact";
				description = json["value"].get<std::string>();

				embed.set_title(title)
					.set_description(description)
					.set_color(0x0099ff);
			}
            else {
				title = "Error";
				description = "Failed to fetch a Chuck Norris fact.";

				embed.set_title(title)
					.set_description(description)
					.set_color(0xff0000);
			}

            event.reply(dpp::message().add_embed(embed), [event](const dpp::confirmation_callback_t& callback) {
                if (callback.is_error()) {
					std::cerr << "Error sending reply: " << callback.get_error().message << std::endl;
				}
                else {
					std::cerr << "Reply sent successfully." << std::endl;
				}
				});
		}
        catch (const std::exception& e) {
			std::cerr << "Exception in handle_chuck_norris_fact_response: " << e.what() << std::endl;
		}
	}
    // Function to handle the response from the number fact API
    void handle_number_fact_response(const dpp::http_request_completion_t& http, dpp::cluster& bot, const dpp::slashcommand_t& event) {
        try {
			dpp::embed embed;
			std::string title, description;

            if (http.status == 200) {
				title = "Random Number Fact";
				description = http.body;

				embed.set_title(title)
					.set_description(description)
					.set_color(0x0099ff);
			}
            else {
				title = "Error";
				description = "Failed to fetch a number fact.";

				embed.set_title(title)
					.set_description(description)
					.set_color(0xff0000);
			}

            event.reply(dpp::message().add_embed(embed), [event](const dpp::confirmation_callback_t& callback) {
                if (callback.is_error()) {
					std::cerr << "Error sending reply: " << callback.get_error().message << std::endl;
				}
                else {
					std::cerr << "Reply sent successfully." << std::endl;
				}
				});
		}
        catch (const std::exception& e) {
			std::cerr << "Exception in handle_number_fact_response: " << e.what() << std::endl;
		}
	}
    // Function to handle the response from the useless fact API
    void handle_useless_fact_response(const dpp::http_request_completion_t& http, dpp::cluster& bot, const dpp::slashcommand_t& event) {
        try {
            dpp::embed embed;
            std::string title, description;

            if (http.status == 200) {
                dpp::json json = dpp::json::parse(http.body);
                title = "Random Useless Fact";
                description = json["text"].get<std::string>();

                embed.set_title(title)
                    .set_description(description)
                    .set_color(0x0099ff);
            }
            else {
                title = "Error";
                description = "Failed to fetch a useless fact.";

                embed.set_title(title)
                    .set_description(description)
                    .set_color(0xff0000);
            }

            event.reply(dpp::message().add_embed(embed), [event](const dpp::confirmation_callback_t& callback) {
                if (callback.is_error()) {
                    std::cerr << "Error sending reply: " << callback.get_error().message << std::endl;
                }
                else {
                    std::cerr << "Reply sent successfully." << std::endl;
                }
                });
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in handle__fact_response: " << e.what() << std::endl;
        }
    }
    // Function to handle the Pierce Brosnan Command
    void handle_pierce_brosnan_fact(const dpp::slashcommand_t& event, dpp::cluster& bot) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, num_pierce_brosnan_facts_count - 1);

		int index = dis(gen);
		std::string fact = pierce_brosnan_facts[index];

		dpp::embed embed;
		embed.set_title("Random Pierce Brosnan Fact")
			.set_description(fact)
			.set_color(0x0099ff);

        event.reply(dpp::message().add_embed(embed), [event](const dpp::confirmation_callback_t& callback) {
            if (callback.is_error()) {
				std::cerr << "Error sending reply: " << callback.get_error().message << std::endl;
			}
            else {
				std::cerr << "Reply sent successfully." << std::endl;
			}
			});
	}

    void handle_facts_command(const dpp::slashcommand_t& event, dpp::cluster& bot) {
        std::string sub_command = event.command.get_command_interaction().options[0].name;

        // Log the sub-command being processed
        std::cerr << "Processing sub-command: " << sub_command << std::endl;

        if (sub_command == "cat") {
            // Log the API request being made
            std::cerr << "Making API request to https://catfact.ninja/fact" << std::endl;

            bot.request("https://catfact.ninja/fact", dpp::http_method::m_get,
                [&bot, event](const dpp::http_request_completion_t& http) mutable { // Capture bot by reference and event by value
                    handle_cat_fact_response(http, bot, event);
                },
                "",
                "text/plain",
                {},
                "1.0"
            );
        }
        else if (sub_command == "dog") {
			// Log the API request being made
			std::cerr << "Making API request to https://dog-api.kinduff.com/api/facts" << std::endl;

			bot.request("https://dog-api.kinduff.com/api/facts", dpp::http_method::m_get,
                [&bot, event](const dpp::http_request_completion_t& http) mutable { // Capture bot by reference and event by value
                	handle_dog_fact_response(http, bot, event);
                },
                "",
                "text/plain",
                {},
                "1.0"
            );
		}
        else if (sub_command == "chucknorris") {
			// Log the API request being made
            std::cerr << "Making API request to https://api.chucknorris.io/jokes/random" << std::endl;

            bot.request("https://api.chucknorris.io/jokes/random", dpp::http_method::m_get,
               	[&bot, event](const dpp::http_request_completion_t& http) mutable { // Capture bot by reference and event by value
               	handle_chuck_norris_fact_response(http, bot, event);
               	},
               	"",
               	"text/plain",
               	{},
               	"1.0"
            );
        }
        else if (sub_command == "number") {
            // Log the API request being made
            std::cerr << "Making API request to http://numbersapi.com/random" << std::endl;

            bot.request("http://numbersapi.com/random", dpp::http_method::m_get,
               [&bot, event](const dpp::http_request_completion_t& http) mutable { // Capture bot by reference and event by value
               handle_number_fact_response(http, bot, event);
               },
               "",
               "text/plain",
               {},
               "1.0"
            );
        }
        else if (sub_command == "useless") {
			// Log the API request being made
			std::cerr << "Making API request to https://uselessfacts.jsph.pl/api/v2/facts/random" << std::endl;

			bot.request("https://uselessfacts.jsph.pl/api/v2/facts/random", dpp::http_method::m_get,
            	[&bot, event](const dpp::http_request_completion_t& http) mutable { // Capture bot by reference and event by value
            	handle_useless_fact_response(http, bot, event);
            	},
            	"",
            	"text/plain",
            	{},
            	"1.0"
            );
		}
        else if (sub_command == "piercebrosnan") {
			handle_pierce_brosnan_fact(event, bot);
		}
    }


}