//
// Created by Berci on 2020. 11. 17..
//

#include "gtest/gtest.h"

#include "data/Base.h"
#include "db/DatabaseImpl.h"

TEST(DatabaseImplTests, TotallyNewDB){
    auto db = S3D::DatabaseImpl::inMemory();

    EXPECT_EQ(db.documents().size(), 0);
}

TEST(DatabaseImplTests, CreateSphere) {
    auto db = S3D::DatabaseImpl::inMemory();

    S3D::IDFactory factory = S3D::IDFactory();

    S3D::ID document = factory();
    S3D::ID entity = factory();
    S3D::Coord coord = S3D::Coord{1,2,3};
    auto radius = S3D::RADIUS{5};

    db.upsert(document, "Test document");

    db.create(entity, S3D::NodeType::Sphere, document);
    db.upsert(entity, coord);
    db.upsert(entity, radius);

    EXPECT_EQ(db.documents().size(), 1);
    auto docs = db.documents();
    EXPECT_EQ(docs.at(0).uid, document);

    auto sphere = db.sphere(entity);
    EXPECT_EQ(sphere->getID(), entity);
    EXPECT_FLOAT_EQ(sphere->radius, radius);
    EXPECT_FLOAT_EQ(sphere->coord.x, coord.x);
    EXPECT_FLOAT_EQ(sphere->coord.y, coord.y);
    EXPECT_FLOAT_EQ(sphere->coord.z, coord.z);
}

TEST(DatabaseImplTests, CreateSetOpNode) {
    auto db = S3D::DatabaseImpl::inMemory();

    S3D::IDFactory factory = S3D::IDFactory();

    S3D::ID document = factory();
    S3D::ID entity = factory();
    auto type = S3D::NodeType::SetOperation;
    auto setop = S3D::SetOperationType::Intersection;

    db.upsert(document, "Test document");

    db.create(entity, type, document);
    db.upsert(entity, setop);

    EXPECT_EQ(db.documents().size(), 1);
    auto docs = db.documents();
    EXPECT_EQ(docs.at(0).uid, document);

    auto node = db.setop(entity);
    EXPECT_EQ(node->getID(), entity);
    EXPECT_EQ(node->type, setop);
}