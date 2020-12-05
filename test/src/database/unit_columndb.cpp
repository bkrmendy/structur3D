//
// Created by Berci on 2020. 12. 04..
//

#include "gtest/gtest.h"

#include "db/ColumnDB.h"
#include "db/column/Column.h"
#include "db/utils/db_utils.h"

TEST(ColumnTests, CreateColumnDB) {
    auto columndb = S3D::ColumnDB{unique_connection_in_memory(false)};
}

TEST(ColumnTests, UpsertInColumnDB) {
    auto columndb = S3D::ColumnDB{unique_connection_in_memory(false)};
    auto entity = S3D::IDFactory()();
    auto radius = S3D::Radius{4};

    columndb.radius.upsert(entity, radius, 1);

    auto radiusFromDB = columndb.radius.latest(entity);

    EXPECT_TRUE(radiusFromDB.has_value());
    EXPECT_EQ(radiusFromDB.value(), radius);
}

TEST(ColumnTests, RetractInColumnDB) {
    auto columndb = S3D::ColumnDB{unique_connection_in_memory(false)};
    auto entity = S3D::IDFactory()();
    auto radius = S3D::Radius{4};

    columndb.radius.upsert(entity, radius, 1);

    columndb.radius.retract(entity, radius, 2);

    auto radiusFromDB = columndb.radius.latest(entity);

    EXPECT_FALSE(radiusFromDB.has_value());
}

TEST(ColumnTests, QueryLatestEntryInColumnDB) {
    auto columndb = S3D::ColumnDB{unique_connection_in_memory(false)};
    auto entity = S3D::IDFactory()();
    auto latest_radius = S3D::Radius{17};

    columndb.radius.upsert(entity, S3D::Radius{9}, 9);
    columndb.radius.upsert(entity, S3D::Radius{4}, 4);
    columndb.radius.upsert(entity, latest_radius, 10);
    columndb.radius.upsert(entity, S3D::Radius{8}, 8);
    columndb.radius.upsert(entity, S3D::Radius{2}, 2);

    auto radiusFromDB = columndb.radius.latest(entity);

    EXPECT_TRUE(radiusFromDB.has_value());
    EXPECT_EQ(radiusFromDB.value(), latest_radius);
}

TEST(ColumnTests, QueryEntriesAfterTimestampInColumnDB) {
    auto columndb = S3D::ColumnDB{unique_connection_in_memory(false)};
    auto entity = S3D::IDFactory()();
    auto latest_radius = S3D::Radius{17};

    columndb.radius.upsert(entity, latest_radius, 10); // <- this should show up
    columndb.radius.upsert(entity, S3D::Radius{9}, 9); // <- this should show up
    columndb.radius.upsert(entity, S3D::Radius{4}, 4);
    columndb.radius.upsert(entity, S3D::Radius{8}, 8);
    columndb.radius.upsert(entity, S3D::Radius{2}, 2);

    auto radiusFromDB = columndb.radius.newer_than(entity, 8);

    EXPECT_EQ(radiusFromDB.size(), 2);
}