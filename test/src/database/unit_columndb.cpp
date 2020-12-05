//
// Created by Berci on 2020. 12. 04..
//

#include "gtest/gtest.h"

#include "db/ColumnDB.h"
#include "db/column/Column.h"
#include "db/db_utils.h"

TEST(ColumnTests, CreateColumnDB) {
    auto columndb = S3D::ColumnDB{unique_connection_in_memory(false)};
}

TEST(ColumnTests, UpsertInColumnDB) {
    auto columndb = S3D::ColumnDB{unique_connection_in_memory(false)};
    FAIL() << "Not implemented";
}

TEST(ColumnTests, RetractInColumnDB) {
    auto columndb = S3D::ColumnDB{unique_connection_in_memory(false)};
    FAIL() << "Not implemented";
}

TEST(ColumnTests, QueryLastEntryInColumnDB) {
    auto columndb = S3D::ColumnDB{unique_connection_in_memory(false)};
    FAIL() << "Not implemented";
}

TEST(ColumnTests, QueryEntriesAfterTimestampInColumnDB) {
    auto columndb = S3D::ColumnDB{unique_connection_in_memory(false)};
    FAIL() << "Not implemented";
}