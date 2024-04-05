#include "db_access.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <string>


MYSQL* conn = nullptr; // MySQL connection handle
const std::chrono::seconds cooldown_duration(120);

bool open_db() {
    // Check if the connection is already open and try to ping the server
    if (conn != nullptr && mysql_ping(conn) == 0) {
        return true; // Connection is alive
    }

    // If the connection is not alive, close the existing connection (if any) and create a new one
    if (conn != nullptr) {
        mysql_close(conn);
    }
    conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, "192.168.1.247", "serena", "92124ABvd@!", "DiscordBotTestServer", 3306, nullptr, 0)) {
        std::cerr << "Can't open database: " << mysql_error(conn) << std::endl;
        return false;
    }
    std::cout << "Opened database successfully" << std::endl;
    return true;
}

void close_db() {
    if (conn != nullptr) {
        mysql_close(conn);
        conn = nullptr;
        std::cout << "Database closed successfully" << std::endl;
    }
}

void execute_sql(const std::string& sql) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    if (mysql_query(conn, sql.c_str())) {
        std::cerr << "SQL Error: " << mysql_error(conn) << std::endl;
    }
    else {
        std::cout << "SQL executed successfully" << std::endl;
    }
}

void execute_sql_with_params(const std::string& sql, const std::vector<std::string>& params) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return;
    }

    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length())) {
        std::cerr << "SQL Error in prepare: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    // Allocate MYSQL_BIND array
    std::vector<MYSQL_BIND> bind(params.size());
    memset(bind.data(), 0, sizeof(MYSQL_BIND) * params.size()); // Initialize to zeros

    // Bind parameters
    for (size_t i = 0; i < params.size(); ++i) {
        bind[i].buffer_type = MYSQL_TYPE_STRING;
        bind[i].buffer = (void*)params[i].c_str();
        bind[i].buffer_length = params[i].length();
    }

    if (mysql_stmt_bind_param(stmt, bind.data())) {
        std::cerr << "SQL Error in bind_param: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "SQL Error in executing: " << mysql_stmt_error(stmt) << std::endl;
    }
    else {
        std::cout << "SQL executed successfully" << std::endl;
    }

    mysql_stmt_close(stmt);
}

void update_guild_info(dpp::snowflake guild_id, const std::string& guild_name, dpp::snowflake owner_id, int member_count) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return;
    }

    const char* sql = "INSERT INTO guild_info (guild_id, guild_name, owner_id, member_count) VALUES (?, ?, ?, ?) ON DUPLICATE KEY UPDATE guild_name = VALUES(guild_name), owner_id = VALUES(owner_id), member_count = VALUES(member_count);";
    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "SQL Error in prepare: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_BIND bind[4] = {};
    memset(bind, 0, sizeof(bind));

    // Binding parameters
    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char*)guild_name.c_str();
    bind[1].buffer_length = guild_name.length();
    bind[2].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[2].buffer = (char*)&owner_id;
    bind[3].buffer_type = MYSQL_TYPE_LONG;
    bind[3].buffer = (char*)&member_count;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "SQL Error in bind_param: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "SQL Error in executing: " << mysql_stmt_error(stmt) << std::endl;
    }
    else {
        std::cout << "Guild info updated successfully" << std::endl;
    }

    mysql_stmt_close(stmt);
}

void update_guild_channel_info(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string& channel_name, const std::string& channel_type) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return;
    }

    const char* sql = "INSERT INTO guild_channels (channel_id, guild_id, channel_name, channel_type) VALUES (?, ?, ?, ?) ON DUPLICATE KEY UPDATE channel_name = VALUES(channel_name), channel_type = VALUES(channel_type);";
    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "SQL Error in prepare: " << mysql_error(conn) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_BIND bind[4];
    memset(bind, 0, sizeof(bind));

    // Prepare data
    long long bind_channel_id = channel_id;
    long long bind_guild_id = guild_id;
    unsigned long name_length = channel_name.length();
    unsigned long type_length = channel_type.length();

    // Bind parameters
    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = &bind_channel_id;
    bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[1].buffer = &bind_guild_id;
    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = (char*)channel_name.c_str();
    bind[2].buffer_length = name_length;
    bind[3].buffer_type = MYSQL_TYPE_STRING;
    bind[3].buffer = (char*)channel_type.c_str();
    bind[3].buffer_length = type_length;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "SQL Error in bind_param: " << mysql_error(conn) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "SQL Error in executing: " << mysql_error(conn) << std::endl;
    }
    else {
        std::cout << "Guild channel information updated successfully" << std::endl;
    }

    mysql_stmt_close(stmt);
}

void update_guild_role_info(dpp::snowflake guild_id, dpp::snowflake role_id, const std::string& role_name) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    // Initialize a new statement
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return;
    }

    // Prepare the SQL statement for replacing role information
    const char* sql = "INSERT INTO guild_roles (guild_id, role_id, role_name) VALUES (?, ?, ?) ON DUPLICATE KEY UPDATE role_name = VALUES(role_name);";
    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "SQL Error in prepare: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt); // Clean up the statement on error
        return;
    }

    // Setup the bind array to bind the parameters to the statement
    MYSQL_BIND bind[3];
    memset(bind, 0, sizeof(bind)); // Initialize bind array

    // Set up bindings for the parameters
    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;
    bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[1].buffer = (char*)&role_id;
    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = (char*)role_name.c_str();
    bind[2].buffer_length = role_name.length();

    // Bind the parameters
    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "SQL Error in bind_param: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt); // Clean up the statement on error
        return;
    }

    // Execute the statement
    if (mysql_stmt_execute(stmt)) {
        std::cerr << "SQL Error in executing: " << mysql_stmt_error(stmt) << std::endl;
    }
    else {
        std::cout << "Guild role information updated successfully" << std::endl;
    }

    // Clean up the statement
    mysql_stmt_close(stmt);
}

void remove_guild_info(dpp::snowflake guild_id) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return;
    }

    std::string sql = "DELETE FROM guild_info WHERE guild_id = ?;";
    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length())) {
        std::cerr << "SQL Error in prepare: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_BIND bind[1];
    memset(&bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "SQL Error in bind_param: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Error deleting guild info: " << mysql_stmt_error(stmt) << std::endl;
    }
    else {
        std::cout << "Guild info deleted successfully" << std::endl;
    }

    mysql_stmt_close(stmt);
}

dpp::snowflake get_warn_channel_id_for_guild(dpp::snowflake guild_id) {
    if (!open_db()) {
		std::cerr << "Failed to open database connection." << std::endl;
		return 0;
	}
    dpp::snowflake channel_id = 0; // Default to 0 (invalid) if not found
    const char* sql = "SELECT channel_id FROM warn_channel_ids WHERE guild_id = ? LIMIT 1;";

    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return 0;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameters: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    MYSQL_BIND result[1];
    memset(result, 0, sizeof(result));

    result[0].buffer_type = MYSQL_TYPE_LONGLONG;
    result[0].buffer = (char*)&channel_id;

    if (mysql_stmt_bind_result(stmt, result)) {
        std::cerr << "Failed to bind result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_fetch(stmt)) {
        std::cerr << "Failed to fetch result: " << mysql_stmt_error(stmt) << std::endl;
        // Even if fetch fails, ensure the statement is properly closed to avoid resource leaks.
    }

    mysql_stmt_close(stmt);
    return channel_id;
}

std::string get_bot_token_from_db() {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return 0;
    }
    std::string token;
    const char* sql = "SELECT token FROM bot_config WHERE name = 'bot_token';";

    if (mysql_query(conn, sql)) {
        std::cerr << "MySQL query error: " << mysql_error(conn) << std::endl;
        return token;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr) {
        std::cerr << "MySQL store result error: " << mysql_error(conn) << std::endl;
        return token;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        token = row[0] ? row[0] : ""; // Check for NULL
    }

    mysql_free_result(result);

    return token;
}

dpp::snowflake get_bot_owner_id() {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return 0;
    }
    dpp::snowflake owner_id = 0;
    const char* sql = "SELECT token FROM bot_config WHERE name = 'bot_owner_id';";

    if (mysql_query(conn, sql)) {
        std::cerr << "MySQL query error: " << mysql_error(conn) << std::endl;
        return owner_id;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr) {
        std::cerr << "MySQL store result error: " << mysql_error(conn) << std::endl;
        return owner_id;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row && row[0]) {
        owner_id = std::stoull(row[0]); // Convert string to unsigned long long
    }

    mysql_free_result(result);

    return owner_id;
}

void add_guild_rule(dpp::snowflake guild_id, const std::string& rule) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    const char* sql = "INSERT INTO guild_rules (guild_id, rule) VALUES (?, ?);";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "SQL Error in prepare: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char*)rule.c_str();
    bind[1].buffer_length = rule.length();

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "SQL Error in bind_param: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Error inserting guild rule: " << mysql_stmt_error(stmt) << std::endl;
    }
    else {
        std::cout << "Guild rule inserted successfully" << std::endl;
    }

    mysql_stmt_close(stmt);
}

std::vector<std::string> get_guild_rules(dpp::snowflake guild_id) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return std::vector<std::string>();
    }
    std::vector<std::string> rules;
    const char* sql = "SELECT rule FROM guild_rules WHERE guild_id = ?;";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return rules;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "SQL Error in prepare: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return rules;
    }

    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "SQL Error in bind_param: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return rules;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "SQL Error in execute: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return rules;
    }

    MYSQL_BIND result[1];
    char rule_buffer[1024]; // Adjust size as needed
    unsigned long length;
    memset(result, 0, sizeof(result));

    result[0].buffer_type = MYSQL_TYPE_STRING;
    result[0].buffer = rule_buffer;
    result[0].buffer_length = sizeof(rule_buffer);
    result[0].length = &length;

    if (mysql_stmt_bind_result(stmt, result)) {
        std::cerr << "MySQL bind error: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return rules;
    }

    if (mysql_stmt_store_result(stmt)) {
        std::cerr << "MySQL store result error: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return rules;
    }

    while (!mysql_stmt_fetch(stmt)) {
        rules.push_back(std::string(rule_buffer, length)); // Create string with correct length
    }

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);

    return rules;
}

void remove_guild_rule(dpp::snowflake guild_id, int rule_number) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    // Adjust for 0-based indexing in SQL
    int offset = rule_number - 1;
    int rule_id = 0;

    // Step 1: Find the rule ID to delete
    std::string sql_select_id = "SELECT id FROM guild_rules WHERE guild_id = ? ORDER BY id LIMIT ?, 1";

    MYSQL_STMT* stmt_select = mysql_stmt_init(conn);
    if (!stmt_select) {
        std::cerr << "Failed to initialize statement handle for select: " << mysql_error(conn) << std::endl;
        return;
    }

    if (mysql_stmt_prepare(stmt_select, sql_select_id.c_str(), sql_select_id.length())) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt_select) << std::endl;
        mysql_stmt_close(stmt_select);
        return;
    }

    MYSQL_BIND bind_select[2];
    memset(bind_select, 0, sizeof(bind_select));

    bind_select[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind_select[0].buffer = (char*)&guild_id;

    bind_select[1].buffer_type = MYSQL_TYPE_LONG;
    bind_select[1].buffer = (char*)&offset;

    if (mysql_stmt_bind_param(stmt_select, bind_select)) {
        std::cerr << "Failed to bind parameters for select: " << mysql_stmt_error(stmt_select) << std::endl;
        mysql_stmt_close(stmt_select);
        return;
    }

    if (mysql_stmt_execute(stmt_select)) {
        std::cerr << "Execution error for select: " << mysql_stmt_error(stmt_select) << std::endl;
        mysql_stmt_close(stmt_select);
        return;
    }

    MYSQL_BIND bind_result[1];
    memset(bind_result, 0, sizeof(bind_result));

    bind_result[0].buffer_type = MYSQL_TYPE_LONG;
    bind_result[0].buffer = (char*)&rule_id;

    if (mysql_stmt_bind_result(stmt_select, bind_result) || mysql_stmt_fetch(stmt_select)) {
        std::cerr << "Fetch error: " << mysql_stmt_error(stmt_select) << std::endl;
    }

    mysql_stmt_close(stmt_select);

    // Step 2: Delete the rule if we found a valid ID
    if (rule_id > 0) {
        std::string sql_delete = "DELETE FROM guild_rules WHERE id = ?";
        MYSQL_STMT* stmt_delete = mysql_stmt_init(conn);

        if (!stmt_delete) {
            std::cerr << "Failed to initialize statement handle for delete: " << mysql_error(conn) << std::endl;
            return;
        }

        if (mysql_stmt_prepare(stmt_delete, sql_delete.c_str(), sql_delete.length())) {
            std::cerr << "Failed to prepare delete statement: " << mysql_stmt_error(stmt_delete) << std::endl;
            mysql_stmt_close(stmt_delete);
            return;
        }

        MYSQL_BIND bind_delete[1];
        memset(bind_delete, 0, sizeof(bind_delete));

        bind_delete[0].buffer_type = MYSQL_TYPE_LONG;
        bind_delete[0].buffer = (char*)&rule_id;

        if (mysql_stmt_bind_param(stmt_delete, bind_delete)) {
            std::cerr << "Failed to bind parameters for delete: " << mysql_stmt_error(stmt_delete) << std::endl;
            mysql_stmt_close(stmt_delete);
            return;
        }

        if (mysql_stmt_execute(stmt_delete)) {
            std::cerr << "Execution error for delete: " << mysql_stmt_error(stmt_delete) << std::endl;
        }
        else {
            std::cout << "Rule successfully removed." << std::endl;
        }

        mysql_stmt_close(stmt_delete);
    }
    else {
        std::cerr << "Rule not found or invalid rule number." << std::endl;
    }
}

dpp::snowflake get_guild_owner_id(dpp::snowflake guild_id) {
    // Ensure the database connection is open and alive
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return 0;
    }
    const char* sql = "SELECT owner_id FROM guild_info WHERE guild_id = ?";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return 0;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "MySQL prepare error: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "MySQL bind_param error: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "MySQL execute error: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    MYSQL_BIND result[1];
    memset(result, 0, sizeof(result));
    dpp::snowflake owner_id = 0;
    result[0].buffer_type = MYSQL_TYPE_LONGLONG;
    result[0].buffer = (char*)&owner_id;

    if (mysql_stmt_bind_result(stmt, result)) {
        std::cerr << "MySQL bind_result error: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_fetch(stmt)) {
        std::cerr << "MySQL fetch error: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    mysql_stmt_close(stmt);
    return owner_id;
}

void set_message_delete_channel(dpp::snowflake guild_id, const std::string& guild_name, dpp::snowflake channel_id) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    const char* sql = "REPLACE INTO message_delete_channels (guild_id, guild_name, channel_id) VALUES (?, ?, ?);";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_BIND bind[3];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char*)guild_name.c_str();
    bind[1].buffer_length = guild_name.length();

    bind[2].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[2].buffer = (char*)&channel_id;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameters: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
    }
    else {
        std::cout << "Message delete channel updated successfully" << std::endl;
    }

    mysql_stmt_close(stmt);
}

void set_warn_channel_for_guild(dpp::snowflake guild_id, dpp::snowflake channel_id) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    const char* sql = "INSERT INTO warn_channel_ids (guild_id, channel_id) VALUES (?, ?) ON DUPLICATE KEY UPDATE channel_id = VALUES(channel_id);";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;
    bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[1].buffer = (char*)&channel_id;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameters: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
    }
    else {
        std::cout << "Warn channel set successfully" << std::endl;
    }

    mysql_stmt_close(stmt);
}

dpp::snowflake get_message_delete_channel_id_for_guild(dpp::snowflake guild_id) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return 0;
    }
    const char* sql = "SELECT channel_id FROM message_delete_channels WHERE guild_id = ?;";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return 0;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    MYSQL_BIND bind[1], result[1];
    memset(bind, 0, sizeof(bind));
    memset(result, 0, sizeof(result));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameter: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    result[0].buffer_type = MYSQL_TYPE_LONGLONG;
    dpp::snowflake channel_id = 0;
    result[0].buffer = (char*)&channel_id;

    if (mysql_stmt_bind_result(stmt, result) || mysql_stmt_execute(stmt) || mysql_stmt_fetch(stmt)) {
        std::cerr << "Failed to fetch result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    mysql_stmt_close(stmt);
    return channel_id;
}

int get_blame_count() {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return 0;
    }
    int count = 0;
    const char* sql = "SELECT count FROM blame_count WHERE id = 1;";

    if (mysql_query(conn, sql)) {
        std::cerr << "MySQL query error: " << mysql_error(conn) << std::endl;
        return count;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr) {
        std::cerr << "MySQL store result error: " << mysql_error(conn) << std::endl;
        return count;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row && row[0]) {
        count = atoi(row[0]); // Convert the result to int
    }

    mysql_free_result(result);
    return count;
}

std::chrono::system_clock::time_point get_last_used_time(dpp::snowflake user_id, const std::string& command) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return std::chrono::system_clock::time_point();  // Return a default-constructed time_point
    }
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return std::chrono::system_clock::time_point();
    }

    const char* sql = "SELECT last_used FROM user_cooldowns WHERE user_id = ? AND command = ?";
    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return std::chrono::system_clock::time_point();
    }

    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&user_id;
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char*)command.c_str();
    bind[1].buffer_length = command.length();

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameters: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return std::chrono::system_clock::time_point();
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return std::chrono::system_clock::time_point();
    }

    long long int last_used = 0;
    MYSQL_BIND result[1];
    memset(result, 0, sizeof(result));
    result[0].buffer_type = MYSQL_TYPE_LONGLONG;
    result[0].buffer = (char*)&last_used;

    if (mysql_stmt_bind_result(stmt, result)) {
        std::cerr << "Failed to bind result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return std::chrono::system_clock::time_point();
    }

    if (mysql_stmt_fetch(stmt)) {
        std::cerr << "Failed to fetch result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return std::chrono::system_clock::time_point();
    }

    mysql_stmt_close(stmt);
    return std::chrono::system_clock::from_time_t(last_used);
}

void update_cooldown(dpp::snowflake user_id, const std::string& command) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    std::string sql = "INSERT INTO user_cooldowns (user_id, command, last_used) VALUES (?, ?, ?) "
        "ON DUPLICATE KEY UPDATE last_used = VALUES(last_used);";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return;
    }

    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length())) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_BIND bind[3];
    memset(bind, 0, sizeof(bind));
    auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&user_id;
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char*)command.c_str();
    bind[1].buffer_length = command.length();
    bind[2].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[2].buffer = (char*)&now;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameters: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
    }
    else {
        std::cout << "Cooldown updated successfully" << std::endl;
    }

    mysql_stmt_close(stmt);
}

std::optional<std::tuple<dpp::snowflake, dpp::snowflake>> get_poll_info(dpp::snowflake guild_id) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return std::nullopt;
    }
    const char* sql = "SELECT message_id, channel_id FROM active_polls WHERE guild_id = ? LIMIT 1;";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return std::nullopt;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return std::nullopt;
    }

    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameter: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return std::nullopt;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return std::nullopt;
    }

    dpp::snowflake message_id = 0, channel_id = 0;
    MYSQL_BIND result[2];
    memset(result, 0, sizeof(result));

    result[0].buffer_type = MYSQL_TYPE_LONGLONG;
    result[0].buffer = (char*)&message_id;
    result[1].buffer_type = MYSQL_TYPE_LONGLONG;
    result[1].buffer = (char*)&channel_id;

    if (mysql_stmt_bind_result(stmt, result)) {
        std::cerr << "Failed to bind result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return std::nullopt;
    }

    if (mysql_stmt_fetch(stmt)) {
        std::cerr << "Failed to fetch result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return std::nullopt;
    }

    mysql_stmt_close(stmt);
    return std::make_optional(std::make_tuple(message_id, channel_id));
}

void remove_poll_info(dpp::snowflake guild_id) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    const char* sql = "DELETE FROM active_polls WHERE guild_id = ?;";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_BIND bind[1];
}

void set_command_enabled_or_disabled_for_guild(dpp::snowflake guild_id, const std::string& command, bool enabled) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    std::string sql = "INSERT INTO guild_commands (guild_id, command, enabled) VALUES (?, ?, ?) "
        "ON DUPLICATE KEY UPDATE enabled = VALUES(enabled);";

    std::vector<std::string> params = { std::to_string(guild_id), command, std::to_string(enabled) };
    execute_sql_with_params(sql, params);
}

void guild_commands_populater(dpp::snowflake guild_id, const std::string& command, bool enabled) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }

    std::string sql = "INSERT IGNORE INTO guild_commands (guild_id, command, enabled) VALUES (?, ?, ?);";

    std::vector<std::string> params = { std::to_string(guild_id), command, std::to_string(enabled) };
    execute_sql_with_params(sql, params);
}

bool is_command_enabled_for_guild(dpp::snowflake guild_id, const std::string& command) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return false;
    }

    const char* sql = "SELECT enabled FROM guild_commands WHERE guild_id = ? AND command = ? LIMIT 1;";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return false;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char*)command.c_str();
    bind[1].buffer_length = command.length();

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameters: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return false;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return false;
    }

    bool enabled = false;
    MYSQL_BIND result[1];
    memset(result, 0, sizeof(result));

    result[0].buffer_type = MYSQL_TYPE_TINY;
    result[0].buffer = (char*)&enabled;

    if (mysql_stmt_bind_result(stmt, result)) {
        std::cerr << "Failed to bind result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return false;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return false;
    }

    int fetch_result = mysql_stmt_fetch(stmt);
    if (fetch_result == MYSQL_NO_DATA) {
        // No data found, implying the command is disabled or not configured for this guild.
        enabled = false;
    }
    else if (fetch_result == 1) {
        // An error occurred during fetch.
        std::cerr << "Failed to fetch result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return false;
    }

    mysql_stmt_close(stmt);
    return enabled;
}

std::string get_guild_name(dpp::snowflake guild_id) {
    std::string guild_name = "";

    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return guild_name;
    }

    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return guild_name;
    }

    std::string sql = "SELECT guild_name FROM guild_info WHERE guild_id = ?";
    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length())) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return guild_name;
    }

    MYSQL_BIND bind[1];
    memset(&bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameters: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return guild_name;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return guild_name;
    }

    // Bind the result
    MYSQL_BIND result[1];
    unsigned long length[1];
    char buffer[256]; // Assuming guild names won't exceed 255 characters
    memset(&result, 0, sizeof(result));
    result[0].buffer_type = MYSQL_TYPE_STRING;
    result[0].buffer = buffer;
    result[0].buffer_length = sizeof(buffer);
    result[0].length = &length[0];

    if (mysql_stmt_bind_result(stmt, result)) {
        std::cerr << "Failed to bind result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return guild_name;
    }

    if (mysql_stmt_fetch(stmt)) {
        std::cerr << "Failed to fetch result: " << mysql_stmt_error(stmt) << std::endl;
    }
    else {
        guild_name = std::string(buffer, *result[0].length);
    }

    mysql_stmt_close(stmt);
    return guild_name;
}

void store_user_join_date(dpp::snowflake guild_id, dpp::snowflake user_id, int64_t join_date) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }

    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return;
    }

    const char* sql = "INSERT INTO guild_member_join_dates (guild_id, user_id, join_date) VALUES (?, ?, ?) ON DUPLICATE KEY UPDATE join_date = VALUES(join_date);";
    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_BIND bind[3];
    memset(&bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;

    bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[1].buffer = (char*)&user_id;

    bind[2].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[2].buffer = (char*)&join_date;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameters: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
    }
    else {
        std::cout << "Join date stored successfully" << std::endl;
    }

    mysql_stmt_close(stmt);
}

std::string get_user_join_date(dpp::snowflake guild_id, dpp::snowflake user_id) {
    std::string join_date = "";

    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return join_date;
    }

    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return join_date;
    }

    const char* sql = "SELECT join_date FROM guild_member_join_dates WHERE guild_id = ? AND user_id = ? LIMIT 1;";
    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return join_date;
    }

    MYSQL_BIND bind[2];
    memset(&bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;

    bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[1].buffer = (char*)&user_id;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameters: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return join_date;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return join_date;
    }

    char buffer[20]; // YYYY-MM-DD HH:MM:SS
    unsigned long length;
    MYSQL_BIND result[1];
    memset(&result, 0, sizeof(result));

    result[0].buffer_type = MYSQL_TYPE_STRING;
    result[0].buffer = buffer;
    result[0].buffer_length = sizeof(buffer);
    result[0].length = &length;

    if (mysql_stmt_bind_result(stmt, result)) {
        std::cerr << "Failed to bind result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return join_date;
    }

    if (mysql_stmt_fetch(stmt) == 0) {
        join_date = std::string(buffer, length);
    }

    mysql_stmt_close(stmt);
    return join_date;
}

void set_member_join_channel_for_guild(dpp::snowflake guild_id, dpp::snowflake channel_id) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return;
    }
    const char* sql = "INSERT INTO guild_member_join_channels (guild_id, channel_id) VALUES (?, ?) ON DUPLICATE KEY UPDATE channel_id = VALUES(channel_id);";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;
    bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[1].buffer = (char*)&channel_id;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameters: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
    }
    else {
        std::cout << "Member join channel set successfully" << std::endl;
    }

    mysql_stmt_close(stmt);
}

dpp::snowflake get_member_join_channel_for_guild(dpp::snowflake guild_id) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return 0;
    }
    const char* sql = "SELECT channel_id FROM guild_member_join_channels WHERE guild_id = ? LIMIT 1;";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return 0;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameter: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    dpp::snowflake channel_id = 0;
    MYSQL_BIND result[1];
    memset(result, 0, sizeof(result));

    result[0].buffer_type = MYSQL_TYPE_LONGLONG;
    result[0].buffer = (char*)&channel_id;

    if (mysql_stmt_bind_result(stmt, result)) {
        std::cerr << "Failed to bind result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_fetch(stmt)) {
        std::cerr << "Failed to fetch result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    mysql_stmt_close(stmt);
    return channel_id;
}

std::string get_openai_api_key() {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return "";
    }
    std::string token;
    const char* sql = "SELECT token FROM bot_config WHERE name = 'openai_api_key';";

    if (mysql_query(conn, sql)) {
        std::cerr << "MySQL query error: " << mysql_error(conn) << std::endl;
        return "";
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr) {
        std::cerr << "MySQL store result error: " << mysql_error(conn) << std::endl;
        return "";
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        token = row[0] ? row[0] : ""; // Check for NULL
    }

    mysql_free_result(result);
    return token;
}

dpp::snowflake get_rules_channel_id(dpp::snowflake guild_id) {
    if (!open_db()) {
        std::cerr << "Failed to open database connection." << std::endl;
        return 0;
    }

    const char* sql = "SELECT channel_id FROM guild_channels WHERE guild_id = ? AND channel_name = 'rules' LIMIT 1;";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "Failed to initialize statement handle: " << mysql_error(conn) << std::endl;
        return 0;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        std::cerr << "Failed to prepare statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = (char*)&guild_id;

    if (mysql_stmt_bind_param(stmt, bind)) {
        std::cerr << "Failed to bind parameter: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    dpp::snowflake channel_id = 0;
    MYSQL_BIND result[1];
    memset(result, 0, sizeof(result));

    result[0].buffer_type = MYSQL_TYPE_LONGLONG;
    result[0].buffer = (char*)&channel_id;

    if (mysql_stmt_bind_result(stmt, result)) {
        std::cerr << "Failed to bind result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_fetch(stmt)) {
        std::cerr << "Failed to fetch result: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return 0;
    }

    mysql_stmt_close(stmt);
    return channel_id;
}