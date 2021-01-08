//
// Created by Berci on 2020. 12. 04..
//

#ifndef STRUCTUR3D_BASE_DB_UTILS_H
#define STRUCTUR3D_BASE_DB_UTILS_H

#include <memory>

#include <sqlpp11/sqlite3/connection.h>

namespace sql = sqlpp::sqlite3;

std::unique_ptr<sql::connection> unique_connection_in_memory(bool debug);
std::unique_ptr<sql::connection> unique_connection_from_path(const char* path);

std::shared_ptr<sql::connection> shared_connection_in_memory(bool debug);
std::shared_ptr<sql::connection> shared_connection_from_path(const char* path);

int is_deleted(bool deleted);
bool is_deleted(int deleted);

#endif //STRUCTUR3D_BASE_DB_UTILS_H
