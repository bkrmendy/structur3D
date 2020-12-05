//
// Created by Berci on 2020. 12. 04..
//

#include "db/db_utils.h"

std::unique_ptr<sql::connection> connection_in_memory(bool debug) {
    sql::connection_config config;
    config.path_to_database = ":memory:";
    config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    config.debug = debug;

    return std::make_unique<sql::connection>(config);
}

std::unique_ptr<sql::connection> connection_from_path(const char* path) {
    sql::connection_config config;
    config.path_to_database = path;
    config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

    return std::make_unique<sql::connection>(config);
}