//
// Created by Berci on 2021. 01. 08..
//

#include "gtest/gtest.h"

#include "db/ColumnDB.h"
#include "db/column/LWWRegister.h"
#include "db/column/LWWElementSet.h"
#include "db/utils/db_utils.h"

TEST(LWWElementSetColumnTests, Upsert) {
    auto columndb = S3D::ColumnDB{unique_connection_in_memory(false)};
    auto makeID = S3D::IDFactory();
    auto this_entity = makeID();
    auto e1 = makeID();
    auto e2 = makeID();
    auto e3 = makeID();

    columndb.edges.upsert(this_entity, e1, 1);
    columndb.edges.upsert(this_entity, e2, 2);
    columndb.edges.upsert(this_entity, e3, 3);

    auto edges_of_this_entity = columndb.edges.latest(this_entity);
    ASSERT_EQ(edges_of_this_entity.size(), 3);
    ASSERT_EQ(edges_of_this_entity.count(e1), 1);
    ASSERT_EQ(edges_of_this_entity.count(e2), 1);
    ASSERT_EQ(edges_of_this_entity.count(e3), 1);
}

TEST(LWWElementSetColumnTests, Retract) {
    // Setup
    auto columndb = S3D::ColumnDB{unique_connection_in_memory(false)};
    auto makeID = S3D::IDFactory();
    auto this_entity = makeID();
    auto e1 = makeID();
    auto e2 = makeID();
    auto e3 = makeID();

    columndb.edges.upsert(this_entity, e1, 1);
    columndb.edges.upsert(this_entity, e2, 1);
    columndb.edges.upsert(this_entity, e3, 1);

    auto edges_of_this_entity = columndb.edges.latest(this_entity);
    ASSERT_EQ(edges_of_this_entity.size(), 3);
    ASSERT_EQ(edges_of_this_entity.count(e1), 1);
    ASSERT_EQ(edges_of_this_entity.count(e2), 1);
    ASSERT_EQ(edges_of_this_entity.count(e3), 1);

    // Retraction
    columndb.edges.retract(this_entity, e1, 2);

    auto edges_of_this_entity_after_retraction = columndb.edges.latest(this_entity);
    ASSERT_EQ(edges_of_this_entity_after_retraction.size(), 2);
    ASSERT_EQ(edges_of_this_entity_after_retraction.count(e1), 0);
    ASSERT_EQ(edges_of_this_entity_after_retraction.count(e2), 1);
    ASSERT_EQ(edges_of_this_entity_after_retraction.count(e3), 1);
}

TEST(LWWElementSetColumnTests, AssertRetractAtSameTime) {
    FAIL() << "Not implemented";
}

TEST(LWWElementSetColumnTests, AssertRetractOutOfOrder) {
    FAIL() << "Not implemented";
}

