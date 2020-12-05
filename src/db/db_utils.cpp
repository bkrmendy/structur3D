//
// Created by Berci on 2020. 12. 04..
//

#include "db/db_utils.h"

sql::connection in_memory(bool debug) {
    sql::connection_config config;
    config.path_to_database = ":memory:";
    config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    config.debug = debug;

    return config;
}

sql::connection from_path(const char* path) {
    sql::connection_config config;
    config.path_to_database = path;
    config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

    return config;
}

std::unique_ptr<sql::connection> unique_connection_from_path(const char* path) {
    return std::make_unique<sql::connection>(from_path(path));
}

std::unique_ptr<sql::connection> unique_connection_in_memory(bool debug) {
    return std::make_unique<sql::connection>(in_memory(debug));
}

std::shared_ptr<sql::connection> shared_connection_in_memory(bool debug) {
    return std::make_shared<sql::connection>(in_memory(debug));
}

std::shared_ptr<sql::connection> shared_connection_from_path(const char* path) {
    return std::make_shared<sql::connection>(from_path(path));
}

int is_deleted(bool deleted) { return deleted ? 1 : 0; }