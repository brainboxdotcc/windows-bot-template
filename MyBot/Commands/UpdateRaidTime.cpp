#include "UpdateRaidTime.h"
#include <dpp/dpp.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <regex>
#include <sstream>

int month_string_to_number(const std::string& month) {
    static const std::unordered_map<std::string, int> month_map = {
        {"January", 1}, {"February", 2}, {"March", 3},
        {"April", 4}, {"May", 5}, {"June", 6},
        {"July", 7}, {"August", 8}, {"September", 9},
        {"October", 10}, {"November", 11}, {"December", 12}
    };
    auto it = month_map.find(month);
    if (it != month_map.end()) {
        return it->second;
    }
    else {
        throw std::invalid_argument("Invalid month string");
    }
}

namespace commands {

    dpp::slashcommand commands::register_updateraidtime_command(dpp::cluster& bot) {
        dpp::slashcommand updateraidtime_command("updateraidtime", "Update the raid time for the server", bot.me.id);
        updateraidtime_command
            .add_option(dpp::command_option(dpp::co_string, "month", "Select the month", true)
                .add_choice(dpp::command_option_choice("January", "January"))
                .add_choice(dpp::command_option_choice("February", "February"))
                .add_choice(dpp::command_option_choice("March", "March"))
                .add_choice(dpp::command_option_choice("April", "April"))
                .add_choice(dpp::command_option_choice("May", "May"))
                .add_choice(dpp::command_option_choice("June", "June"))
                .add_choice(dpp::command_option_choice("July", "July"))
                .add_choice(dpp::command_option_choice("August", "August"))
                .add_choice(dpp::command_option_choice("September", "September"))
                .add_choice(dpp::command_option_choice("October", "October"))
                .add_choice(dpp::command_option_choice("November", "November"))
                .add_choice(dpp::command_option_choice("December", "December"))
            )
            .add_option(dpp::command_option(dpp::co_integer, "day", "Enter the day", true))
            .add_option(dpp::command_option(dpp::co_integer, "year", "Select the year", true)
                .add_choice(dpp::command_option_choice("2024", "2024"))
                .add_choice(dpp::command_option_choice("2025", "2025"))
                .add_choice(dpp::command_option_choice("2026", "2026"))
                .add_choice(dpp::command_option_choice("2027", "2027"))
                .add_choice(dpp::command_option_choice("2028", "2028"))
                .add_choice(dpp::command_option_choice("2029", "2029"))
                .add_choice(dpp::command_option_choice("2030", "2030"))
            )
            .add_option(dpp::command_option(dpp::co_string, "time", "Select the time", true)
                .add_choice(dpp::command_option_choice("1:00 AM", "01:00 AM"))
                .add_choice(dpp::command_option_choice("2:00 AM", "02:00 AM"))
                .add_choice(dpp::command_option_choice("3:00 AM", "03:00 AM"))
                .add_choice(dpp::command_option_choice("4:00 AM", "04:00 AM"))
                .add_choice(dpp::command_option_choice("5:00 AM", "05:00 AM"))
                .add_choice(dpp::command_option_choice("6:00 AM", "06:00 AM"))
                .add_choice(dpp::command_option_choice("7:00 AM", "07:00 AM"))
                .add_choice(dpp::command_option_choice("8:00 AM", "08:00 AM"))
                .add_choice(dpp::command_option_choice("9:00 AM", "09:00 AM"))
                .add_choice(dpp::command_option_choice("10:00 AM", "10:00 AM"))
                .add_choice(dpp::command_option_choice("11:00 AM", "11:00 AM"))
                .add_choice(dpp::command_option_choice("12:00 PM", "12:00 PM"))
                .add_choice(dpp::command_option_choice("1:00 PM", "1:00 PM"))
                .add_choice(dpp::command_option_choice("2:00 PM", "2:00 PM"))
                .add_choice(dpp::command_option_choice("3:00 PM", "3:00 PM"))
                .add_choice(dpp::command_option_choice("4:00 PM", "4:00 PM"))
                .add_choice(dpp::command_option_choice("5:00 PM", "5:00 PM"))
                .add_choice(dpp::command_option_choice("6:00 PM", "6:00 PM"))
                .add_choice(dpp::command_option_choice("7:00 PM", "7:00 PM"))
                .add_choice(dpp::command_option_choice("8:00 PM", "8:00 PM"))
                .add_choice(dpp::command_option_choice("9:00 PM", "9:00 PM"))
                .add_choice(dpp::command_option_choice("10:00 PM", "10:00 PM"))
                .add_choice(dpp::command_option_choice("11:00 PM", "11:00 PM"))
            )
            .add_option(dpp::command_option(dpp::co_string, "timezone", "Select the timezone", true)
                .add_choice(dpp::command_option_choice("Eastern Time", "America/New_York"))
                .add_choice(dpp::command_option_choice("Central Time", "America/Chicago"))
                .add_choice(dpp::command_option_choice("Mountain Time", "America/Denver"))
                .add_choice(dpp::command_option_choice("Pacific Time", "America/Los_Angeles"))
                .add_choice(dpp::command_option_choice("Alaska Time", "America/Anchorage"))
                .add_choice(dpp::command_option_choice("Hawaii Standard Time", "America/Honolulu"))
            )
            .add_option(dpp::command_option(dpp::co_string, "raid", "Select the raid", true)
                .add_choice(dpp::command_option_choice("Alexander - The Burden of the Son (Savage)", "Alexander - The Burden of the Son (Savage)"))
                .add_choice(dpp::command_option_choice("Alexander - The Eyes of the Creator (Savage)", "Alexander - The Eyes of the Creator (Savage)"))
                .add_choice(dpp::command_option_choice("Alexander - The Breath of the Creator (Savage)", "Alexander - The Breath of the Creator (Savage)"))
                .add_choice(dpp::command_option_choice("Alexander - The Heart of the Creator (Savage)", "Alexander - The Heart of the Creator (Savage)"))
                .add_choice(dpp::command_option_choice("Alexander - The Soul of the Creator (Savage)", "Alexander - The Soul of the Creator (Savage)"))

            )
            .add_option(dpp::command_option(dpp::co_channel, "channel", "Select the channel", true))
            .add_option(dpp::command_option(dpp::co_boolean, "mine", "Is this M.I.N.E. or not?", false));

        // Return the command object
        return updateraidtime_command;
    }
    
    void handle_updateraidtime_command(const dpp::slashcommand_t& event, dpp::cluster& bot) {
        std::string month = std::get<std::string>(event.get_parameter("month"));
        int day = std::get<int64_t>(event.get_parameter("day"));
        int year = std::get<int64_t>(event.get_parameter("year"));
        std::string time = std::get<std::string>(event.get_parameter("time"));
        std::string timezone = std::get<std::string>(event.get_parameter("timezone"));
        std::string raid = std::get<std::string>(event.get_parameter("raid"));
        dpp::snowflake channel_id = std::get<dpp::snowflake>(event.get_parameter("channel"));
 
        bool mine = false; // Default to false if not provided
        auto mine_option = event.get_parameter("mine"); // Fetch the 'mine' parameter, if present
        if (std::holds_alternative<bool>(mine_option)) { // Check if 'mine' was provided and is a bool
            mine = std::get<bool>(mine_option); // Safe to get the value since we know it's a bool
        }

        // Parse the time string
        std::regex time_regex(R"((\d{1,2}):(\d{2}) (AM|PM))");
        std::smatch matches;
        if (!std::regex_search(time, matches, time_regex)) {
            std::cout << "Invalid time format. Please use the format HH:MM AM/PM." << std::endl;
            return;
        }
        int hour = std::stoi(matches[1].str());
        int minute = std::stoi(matches[2].str());
        std::string am_pm = matches[3].str();
        if (am_pm == "PM" && hour != 12) {
            hour += 12;
        }
        else if (am_pm == "AM" && hour == 12) {
            hour = 0;
        }

        // Create a tm structure for the specified date and time
        std::tm tm = {};
        tm.tm_year = year - 1900;
        tm.tm_mon = month_string_to_number(month) - 1;
        tm.tm_mday = day;
        tm.tm_hour = hour;
        tm.tm_min = minute;
        tm.tm_sec = 0;
        tm.tm_isdst = -1; // Let mktime() determine if DST is in effect

        std::cout << "Initial tm structure: "
            << "Year: " << tm.tm_year + 1900
            << ", Month: " << tm.tm_mon + 1
            << ", Day: " << tm.tm_mday
            << ", Hour: " << tm.tm_hour
            << ", Minute: " << tm.tm_min << std::endl;

        // Convert the tm structure to a time_t value
        std::time_t time_t_value = std::mktime(&tm);

        // Log the local time and time_t value
        char buffer[26];
        asctime_s(buffer, sizeof(buffer), &tm);
        std::cout << "Local time: " << buffer << "time_t value: " << time_t_value << std::endl;

        // Log the adjusted time_t value for timezone
        std::tm adjusted_tm = {};
        gmtime_s(&adjusted_tm, &time_t_value);
        asctime_s(buffer, sizeof(buffer), &adjusted_tm);
        std::cout << "Adjusted local time: " << buffer << "Adjusted time_t value for timezone: " << time_t_value << std::endl;

        // Log the final time_t value after timezone adjustment
        gmtime_s(&adjusted_tm, &time_t_value);
        asctime_s(buffer, sizeof(buffer), &adjusted_tm);
        std::cout << "Final adjusted local time: " << buffer << "Final time_t value after timezone adjustment: " << time_t_value << std::endl;

        // Convert to a Unix timestamp
        long long unix_timestamp = static_cast<long long>(time_t_value);

        // Log the final Unix timestamp
        std::cout << "Unix timestamp: " << unix_timestamp << std::endl;

        // Update the topic of the channel
        std::string topic = "Next Meet Is: " + raid;
        if (mine) {
            topic += " M.I.N.E.";
        }
        topic += " | Time: <t:" + std::to_string(unix_timestamp) + ":f>";

        // Use shared_ptr to protect the lambda from potential issues
        auto response = std::make_shared<dpp::slashcommand_t>(event);

        // Fetch the current channel information
        bot.channel_get(channel_id, [response, &bot, topic](const dpp::confirmation_callback_t& confirmation) {
            if (confirmation.is_error()) {
                // Handle error in fetching channel info
                auto error = confirmation.get_error();
                std::string error_message = "Failed to fetch channel info: Code " + std::to_string(error.code) + " - " + error.message;
                bot.log(dpp::ll_error, error_message);
                response->reply(error_message);
            }
            else {
                // Get the current channel object
                dpp::channel current_channel = std::get<dpp::channel>(confirmation.value);

                // Create a new channel object with updated topic
                dpp::channel updated_channel = current_channel; // Copy the current channel
                updated_channel.set_topic(topic); // Update the topic

                // Call channel_edit with the updated channel object
                bot.channel_edit(updated_channel, [response, &bot, topic](const dpp::confirmation_callback_t& confirmation) {
                    if (confirmation.is_error()) {
                        // Handle error in updating channel topic
                        auto error = confirmation.get_error();
                        std::string error_message = "Failed to update channel topic: Code " + std::to_string(error.code) + " - " + error.message;
                        bot.log(dpp::ll_error, error_message);
                        response->reply(error_message);
                    }
                    else {
                        // Successfully updated the channel topic
                        response->reply(topic);
                    }
                    });
            }
            });
    }



}