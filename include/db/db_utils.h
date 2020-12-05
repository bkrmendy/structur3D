//
// Created by Berci on 2020. 12. 04..
//

#ifndef STRUCTUR3D_BASE_DB_UTILS_H
#define STRUCTUR3D_BASE_DB_UTILS_H

#include <memory>

#include <sqlpp11/sqlite3/connection.h>

namespace sql = sqlpp::sqlite3;

std::unique_ptr<sql::connection> connection_in_memory(bool debug);
std::unique_ptr<sql::connection> connection_from_path(const char* path);

#endif //STRUCTUR3D_BASE_DB_UTILS_H
