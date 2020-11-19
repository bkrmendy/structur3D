//
// Created by Berci on 2020. 11. 17..
//

#include "gtest/gtest.h"

#include "data/Base.h"
#include "db/DatabaseImpl.h"

TEST(DatabaseImplTests, TotallyNewDB){
    auto db = S3D::DatabaseImpl::inMemory(false);

    EXPECT_TRUE(db.documents().empty());
}

TEST(DatabaseImplTests, CreateSphere) {
    auto db = S3D::DatabaseImpl::inMemory(false);

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
    auto db = S3D::DatabaseImpl::inMemory(false);

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

TEST(DatabaseImplTests, RetractSphereNode) {
    auto db = S3D::DatabaseImpl::inMemory(false);

    S3D::IDFactory factory = S3D::IDFactory();

    S3D::ID document = factory();
    S3D::ID entity = factory();
    S3D::Coord coord = S3D::Coord{1,2,3};
    auto radius = S3D::RADIUS{5};

    db.upsert(document, "Test document");

    db.create(entity, S3D::NodeType::Sphere, document);
    db.upsert(entity, coord);
    db.upsert(entity, radius);

    db.retract(entity, coord);
    db.retract(entity, radius);
    db.remove(entity, document);

    EXPECT_TRUE(db.documents().empty());
    EXPECT_EQ(db.sphere(entity), std::nullopt);
}

TEST(DatabaseImplTests, RetractSetNode) {
    auto db = S3D::DatabaseImpl::inMemory(false);

    S3D::IDFactory factory = S3D::IDFactory();

    S3D::ID document = factory();
    S3D::ID entity = factory();
    auto type = S3D::NodeType::SetOperation;
    auto setop = S3D::SetOperationType::Intersection;

    db.upsert(document, "Test document");

    db.create(entity, type, document);
    db.upsert(entity, setop);

    db.retract(entity, setop);
    db.remove(entity, document);

    EXPECT_TRUE(db.documents().empty());
    EXPECT_EQ(db.setop(entity), std::nullopt);
}

TEST(DatabaseImplTests, LookupNonExistentSphere) {
    auto db = S3D::DatabaseImpl::inMemory(false);
    auto factory = S3D::IDFactory();
    auto entityFromThinAir = factory();

    EXPECT_EQ(db.sphere(entityFromThinAir), std::nullopt);
}

TEST(DatabaseImplTests, LookUpNonExistentSetNode) {
    auto db = S3D::DatabaseImpl::inMemory(false);
    auto factory = S3D::IDFactory();
    auto entityFromThinAir = factory();

    EXPECT_EQ(db.setop(entityFromThinAir), std::nullopt);
}

TEST(DatabaseImplTests, LookupSphereWithoutCoord) {
    auto db = S3D::DatabaseImpl::inMemory(false);

    S3D::IDFactory factory = S3D::IDFactory();

    S3D::ID document = factory();
    S3D::ID entity = factory();
    auto radius = S3D::RADIUS{5};

    db.upsert(document, "Test document");

    db.create(entity, S3D::NodeType::Sphere, document);
    db.upsert(entity, radius);

    EXPECT_EQ(db.sphere(entity), std::nullopt);
}

TEST(DatabaseImplTests, LookupSphereWithoutRadius) {
    auto db = S3D::DatabaseImpl::inMemory(false);

    S3D::IDFactory factory = S3D::IDFactory();

    S3D::ID document = factory();
    S3D::ID entity = factory();
    auto coord = S3D::Coord{1,2,3};

    db.upsert(document, "Test document");

    db.create(entity, S3D::NodeType::Sphere, document);
    db.upsert(entity, coord);

    EXPECT_EQ(db.sphere(entity), std::nullopt);
}

TEST(DatabaseImplTests, LookUpIncompleteSetNode) {
    auto db = S3D::DatabaseImpl::inMemory(false);

    S3D::IDFactory factory = S3D::IDFactory();

    S3D::ID document = factory();
    S3D::ID entity = factory();
    auto type = S3D::NodeType::SetOperation;

    db.upsert(document, "Test document");

    db.create(entity, type, document);

    EXPECT_EQ(db.setop(entity), std::nullopt);
}

TEST(DatabaseImplTests, LookupDocumentWithoutName) {
    auto db = S3D::DatabaseImpl::inMemory(false);
    auto makeID = S3D::IDFactory();

    auto document = makeID();
    auto dummy = makeID();

    db.create(dummy, S3D::NodeType::Sphere, document);

    EXPECT_TRUE(db.documents().empty());
}

TEST(DatabaseImplTests, CreateAndReadSimpleGraph) {
    auto db = S3D::DatabaseImpl::inMemory(false);
    auto makeID = S3D::IDFactory();

    auto document = makeID();
    db.upsert(document, "Test Document");

    auto unionNode = makeID();
    db.create(unionNode, S3D::NodeType::SetOperation, document);
    db.upsert(unionNode, S3D::SetOperationType::Union);

    auto s1 = makeID();
    db.create(s1, S3D::NodeType::Sphere, document);
    db.upsert(s1, S3D::Coord{-1, -2, -3});
    db.upsert(s1, S3D::RADIUS{2});

    auto s2 = makeID();
    db.create(s2, S3D::NodeType::Sphere, document);
    db.upsert(s2, S3D::Coord{1, 2, 3});
    db.upsert(s2, S3D::RADIUS{3});

    db.connect(unionNode, s1);
    db.connect(unionNode, s2);

    EXPECT_EQ(db.documents().size(), 1);
    EXPECT_EQ(db.entites(document).size(), 3);

    EXPECT_TRUE(db.setop(unionNode).has_value());
    EXPECT_TRUE(db.sphere(s1).has_value());
    EXPECT_TRUE(db.sphere(s2).has_value());

    EXPECT_EQ(db.edges(unionNode).size(), 2);
    EXPECT_TRUE(db.edges(s1).empty());
    EXPECT_TRUE(db.edges(s2).empty());

    auto unionNodeFromDb = db.setop(unionNode);
    EXPECT_EQ(unionNodeFromDb->type, S3D::SetOperationType::Union);

    auto s1FromDb = db.sphere(s1);
    EXPECT_EQ(s1FromDb->radius, 2);
    EXPECT_FLOAT_EQ(s1FromDb->coord.x, -1);
    EXPECT_FLOAT_EQ(s1FromDb->coord.y, -2);
    EXPECT_FLOAT_EQ(s1FromDb->coord.z, -3);

    auto s2FromDb = db.sphere(s2);
    EXPECT_EQ(s2FromDb->radius, 3);
    EXPECT_FLOAT_EQ(s2FromDb->coord.x, 1);
    EXPECT_FLOAT_EQ(s2FromDb->coord.y, 2);
    EXPECT_FLOAT_EQ(s2FromDb->coord.z, 3);

}

TEST(DatabaseImplTests, ReadEdges) {
    auto db = S3D::DatabaseImpl::inMemory(false);
    S3D::IDFactory factory = S3D::IDFactory();

    auto e1 = factory();
    auto e2 = factory();

    db.connect(e1, e2);

    auto edges_of_e1 = db.edges(e1);
    auto edges_of_e2 = db.edges(e2);

    EXPECT_EQ(edges_of_e1.size(), 1);
    EXPECT_EQ(edges_of_e1.at(0), e2);
    EXPECT_TRUE(edges_of_e2.empty());
}

TEST(DatabaseImplTests, DisconnectEdges) {
    auto db = S3D::DatabaseImpl::inMemory(false);
    S3D::IDFactory factory = S3D::IDFactory();

    auto e1 = factory();
    auto e2 = factory();
    auto e3 = factory();

    db.connect(e1, e2);
    db.connect(e1, e3);

    EXPECT_EQ(db.edges(e1).size(), 2);

    db.disconnect(e1, e2);

    EXPECT_EQ(db.edges(e1).size(), 1);

    EXPECT_EQ(db.edges(e1).at(0), e3);
}

TEST(DatabaseImplTests, DisconnectNonExistentEdges) {
    auto db = S3D::DatabaseImpl::inMemory(false);
    S3D::IDFactory factory = S3D::IDFactory();

    auto e1 = factory();
    auto e2 = factory();
    auto e3 = factory();

    db.connect(e1, e2);
    db.connect(e1, e3);

    EXPECT_EQ(db.edges(e1).size(), 2);
    EXPECT_TRUE(db.edges(e2).empty());
    EXPECT_TRUE(db.edges(e3).empty());

    db.disconnect(e2, e3);

    EXPECT_EQ(db.edges(e1).size(), 2);
    EXPECT_TRUE(db.edges(e2).empty());
    EXPECT_TRUE(db.edges(e3).empty());
}

TEST(DatabaseImplTests, NodeTypeConversionTests) {
    EXPECT_EQ(S3D::to_integral(S3D::NodeType::Sphere), 0);
    EXPECT_EQ(S3D::to_integral(S3D::NodeType::SetOperation), 1);

    EXPECT_EQ(S3D::from_integral(0), S3D::NodeType::Sphere);
    EXPECT_EQ(S3D::from_integral(1), S3D::NodeType::SetOperation);

    for (size_t i = 2; i < 100; ++i) {
        EXPECT_EQ(S3D::from_integral(i), std::nullopt);
    }
}

TEST(DatabaseImplTests, RadiusCRDTProperty) {
    FAIL() << "Not implemented";
}

TEST(DatabaseImplTests, CoordinateCRDTProperty) {
    FAIL() << "Not implemented";
}

TEST(DatabaseImplTests, SetOperationTypeCRDTProperty) {
    FAIL() << "Not implemented";
}

TEST(DatabaseImplTests, NameCRDTProperty) {
    FAIL() << "Not implemented";
}

