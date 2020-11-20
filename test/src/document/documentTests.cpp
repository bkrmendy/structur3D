//
// Created by Berci on 2020. 11. 19..
//

#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "data/Edge.h"
#include "document/DocumentImpl.h"
#include "db/Database.h"

namespace S3D {
    class MockDatabase : public S3D::Database {
    public:
        MOCK_METHOD(void, upsert, (const ID&, const RADIUS&), (override));
        MOCK_METHOD(void, retract, (const ID&, const RADIUS&), (override));

        MOCK_METHOD(void, upsert, (const ID&, const Coord&), (override));
        MOCK_METHOD(void, retract, (const ID&, const Coord&), (override));

        MOCK_METHOD(void, upsert, (const ID&, const SetOperationType&), (override));
        MOCK_METHOD(void, retract, (const ID&, const SetOperationType&), (override));

        MOCK_METHOD(void, upsert, (const ID&, const std::string& name), (override));
        MOCK_METHOD(void, retract, (const ID&, const std::string& name), (override));

        MOCK_METHOD(void, connect, (const ID&, const ID&), (override));
        MOCK_METHOD(void, disconnect, (const ID&, const ID&), (override));

        MOCK_METHOD(void, create, (const ID& entity, const NodeType& type, const ID& document), (override));
        MOCK_METHOD(void, remove, (const ID& entity, const ID& document), (override));

        MOCK_METHOD(std::vector<DocumentWithName>, documents, (), (override));
        MOCK_METHOD(std::vector<IDWithType>, entities, (const ID&), (override));
        MOCK_METHOD(std::optional<Sphere>, sphere, (const ID&), (override));
        MOCK_METHOD(std::optional<SetOp>, setop, (const ID& from), (override));
        MOCK_METHOD(std::vector<ID>, edges, (const ID&), (override));
    };

    class MockMeshFactory : public MeshFactory {
        std::unique_ptr<Mesh> fromTree(const std::vector<Tree>&) {
            std::vector<S3DVertex> vertices{};
            std::vector<uint32_t> indices{};
            return std::make_unique<Mesh>(vertices, indices);
        }
    };
}

TEST(DocumentTests, PropertyAccessorsOK) {

    std::shared_ptr<S3D::MockDatabase> db = std::make_shared<S3D::MockDatabase>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);

    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::RADIUS{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::RADIUS{7});

    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);

    std::vector<std::shared_ptr<S3D::Edge>> edges = { edge1, edge2 };
    std::vector<std::shared_ptr<S3D::Node>> nodes = { unionNode, sphere1, sphere2 };

    auto doc = S3D::DocumentImpl{docId, db, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>()};

    EXPECT_EQ(doc.id(), docId);
    EXPECT_EQ(doc.graph()->edges, edges);
    EXPECT_EQ(doc.graph()->nodes, nodes);
}

TEST(DocumentTests, UpdateSphereRadius) {
    std::shared_ptr<S3D::MockDatabase> db = std::make_shared<S3D::MockDatabase>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto sphere = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::RADIUS{4});
    std::vector<std::shared_ptr<S3D::Edge>> edges{};
    std::vector<std::shared_ptr<S3D::Node>> nodes = { sphere };

    auto doc = S3D::DocumentImpl(docId, db, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    auto new_radius = S3D::RADIUS{100};
    EXPECT_CALL(*db, upsert(sphere->id(), new_radius)).Times(1);

    sphere->radius = new_radius;
    doc.update(sphere);

    EXPECT_EQ(std::dynamic_pointer_cast<S3D::Sphere>(doc.graph()->nodes.at(0))->radius, new_radius);
}

TEST(DocumentTests, UpdateSphereCoords) {
    std::shared_ptr<S3D::MockDatabase> db = std::make_shared<S3D::MockDatabase>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto sphere = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::RADIUS{4});
    std::vector<std::shared_ptr<S3D::Edge>> edges{};
    std::vector<std::shared_ptr<S3D::Node>> nodes = { sphere };

    auto doc = S3D::DocumentImpl(docId, db, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    auto new_coord = S3D::Coord{100, 200, 300};
    // todo: EXPECT CALL

    sphere->coord = new_coord;
    doc.update(sphere);

    auto actualCoord = std::dynamic_pointer_cast<S3D::Sphere>(doc.graph()->nodes.at(0))->coord;
    EXPECT_FLOAT_EQ(actualCoord.x, new_coord.x);
    EXPECT_FLOAT_EQ(actualCoord.y, new_coord.y);
    EXPECT_FLOAT_EQ(actualCoord.z, new_coord.z);
}

TEST(DocumentTests, CreateSetOp) {
    std::shared_ptr<S3D::MockDatabase> db = std::make_shared<S3D::MockDatabase>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    std::vector<std::shared_ptr<S3D::Edge>> edges{};
    std::vector<std::shared_ptr<S3D::Node>> nodes{};

    auto doc = S3D::DocumentImpl(docId, db, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    auto setop = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Intersection);

    EXPECT_CALL(*db, create(setop->id(), S3D::NodeType::SetOperation, docId)).Times(1);
    EXPECT_CALL(*db, upsert(setop->id(), setop->type)).Times(1);
    // todo: EXPECT CALL coord

    doc.create(setop);

    EXPECT_EQ(doc.graph()->nodes.size(), 1);
    EXPECT_EQ(doc.graph()->nodes.at(0)->id(), setop->id());
}

TEST(DocumentTests, CreateSphere) {
    std::shared_ptr<S3D::MockDatabase> db = std::make_shared<S3D::MockDatabase>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    std::vector<std::shared_ptr<S3D::Edge>> edges{};
    std::vector<std::shared_ptr<S3D::Node>> nodes{};

    auto doc = S3D::DocumentImpl(docId, db, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    auto sphere = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::RADIUS{4});

    EXPECT_CALL(*db, create(sphere->id(), S3D::NodeType::Sphere, docId)).Times(1);
    EXPECT_CALL(*db, upsert(sphere->id(), sphere->radius)).Times(1);
    // todo: EXPECT CALL coord

    doc.create(sphere);

    EXPECT_EQ(doc.graph()->nodes.size(), 1);
    EXPECT_EQ(doc.graph()->nodes.at(0)->id(), sphere->id());
}

TEST(DocumentTests, RemoveSetOp) {
    std::shared_ptr<S3D::MockDatabase> db = std::make_shared<S3D::MockDatabase>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::RADIUS{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::RADIUS{7});
    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);

    std::vector<std::shared_ptr<S3D::Edge>> edges = {edge1, edge2};

    std::vector<std::shared_ptr<S3D::Node>> nodes = {unionNode, sphere1, sphere2};

    auto doc = S3D::DocumentImpl(docId, db, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    EXPECT_CALL(*db, remove(unionNode->id(), docId));
    EXPECT_CALL(*db, retract(unionNode->id(), S3D::SetOperationType::Union));

    doc.remove(unionNode);

    EXPECT_EQ(doc.graph()->nodes.size(), 2);
    EXPECT_TRUE(doc.graph()->edges.empty());
}

TEST(DocumentTests, RemoveSphere) {
    std::shared_ptr<S3D::MockDatabase> db = std::make_shared<S3D::MockDatabase>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::RADIUS{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::RADIUS{7});
    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);

    std::vector<std::shared_ptr<S3D::Edge>> edges = {edge1, edge2};

    std::vector<std::shared_ptr<S3D::Node>> nodes = {unionNode, sphere1, sphere2};

    auto doc = S3D::DocumentImpl(docId, db, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    EXPECT_CALL(*db, remove(sphere1->id(), docId));
    EXPECT_CALL(*db, retract(sphere1->id(), sphere1->radius));
    // TODO: expect call coord

    doc.remove(sphere1);

    EXPECT_EQ(doc.graph()->nodes.size(), 2);
    EXPECT_EQ(doc.graph()->edges.size(), 1);
    EXPECT_EQ(doc.graph()->edges.at(0)->from->id(), unionNode->id());
    EXPECT_EQ(doc.graph()->edges.at(0)->to->id(), sphere2->id());
}

TEST(DocumentTest, CreateEdge) {
    std::shared_ptr<S3D::MockDatabase> db = std::make_shared<S3D::MockDatabase>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::RADIUS{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::RADIUS{7});

    std::vector<std::shared_ptr<S3D::Edge>> edges = {};

    std::vector<std::shared_ptr<S3D::Node>> nodes = {unionNode, sphere1, sphere2};

    auto doc = S3D::DocumentImpl(docId, db, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    EXPECT_CALL(*db, connect(unionNode->id(), sphere1->id())).Times(1);

    doc.create(std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1));

    EXPECT_EQ(doc.graph()->nodes.size(), 3);
    EXPECT_EQ(doc.graph()->edges.size(), 1);
    EXPECT_EQ(doc.graph()->edges.at(0)->from, unionNode);
    EXPECT_EQ(doc.graph()->edges.at(0)->to, sphere1);
}

TEST(DocumentTest, RemoveEdge) {
    std::shared_ptr<S3D::MockDatabase> db = std::make_shared<S3D::MockDatabase>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::RADIUS{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::RADIUS{7});
    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);

    std::vector<std::shared_ptr<S3D::Edge>> edges = {edge1, edge2};

    std::vector<std::shared_ptr<S3D::Node>> nodes = {unionNode, sphere1, sphere2};

    auto doc = S3D::DocumentImpl(docId, db, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    EXPECT_CALL(*db, disconnect(unionNode->id(), sphere1->id()));

    doc.remove(edges.at(0));

    EXPECT_EQ(doc.graph()->nodes.size(), 3);
    EXPECT_EQ(doc.graph()->edges.size(), 1);
    EXPECT_EQ(doc.graph()->edges.at(0)->from, unionNode);
    EXPECT_EQ(doc.graph()->edges.at(0)->to, sphere2);

}


