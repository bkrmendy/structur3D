//
// Created by Berci on 2020. 11. 19..
//

#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "data/Edge.h"
#include "document/DocumentImpl.h"
#include "db/Database.h"
#include "data/Attribute.h"

namespace S3D {
    class MockDatabase : public S3D::Database {
    public:
        MOCK_METHOD(void, upsert, (const ID&, const Radius&, Timestamp timestamp), (override));
        MOCK_METHOD(void, retract, (const ID&, const Radius&, Timestamp timestamp), (override));

        MOCK_METHOD(void, upsert, (const ID&, const Coord&, Timestamp timestamp), (override));
        MOCK_METHOD(void, retract, (const ID&, const Coord&, Timestamp timestamp), (override));

        MOCK_METHOD(void, upsert, (const ID&, const SetOperationType&, Timestamp timestamp), (override));
        MOCK_METHOD(void, retract, (const ID&, const SetOperationType&, Timestamp timestamp), (override));

        MOCK_METHOD(void, upsert, (const ID&, const Name& name, Timestamp timestamp), (override));
        MOCK_METHOD(void, retract, (const ID&, const Name& name, Timestamp timestamp), (override));

        MOCK_METHOD(void, connect, (const ID&, const ID&, Timestamp timestamp), (override));
        MOCK_METHOD(void, disconnect, (const ID&, const ID&, Timestamp timestamp), (override));

        MOCK_METHOD(void, create, (const ID& entity, const NodeType& type, const ID& document, Timestamp timestamp), (override));
        MOCK_METHOD(void, remove, (const ID& entity, const ID& document, Timestamp timestamp), (override));

        MOCK_METHOD(std::vector<DocumentWithName>, documents, (), (override));
        MOCK_METHOD(std::vector<IDWithType>, entities, (const ID&), (override));
        MOCK_METHOD(std::optional<Sphere>, sphere, (const ID&), (override));
        MOCK_METHOD(std::optional<SetOp>, setop, (const ID& from), (override));
        MOCK_METHOD(std::vector<ID>, edges, (const ID&), (override));
    };

    class MockMeshFactory : public MeshFactory {
        std::unique_ptr<Mesh> fromTree(const std::vector<Tree>&) {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
            return std::make_unique<Mesh>(vertices, indices);
        }
    };

    class MockInteractor : public Interactor {
    public:
        MOCK_METHOD(void, create, (std::shared_ptr<Node>, const ID& document), (override));
        MOCK_METHOD(void, connect, (const ID& from, const ID& to), (override));
        MOCK_METHOD(void, update, (const ID& entity, const Attribute& attribute), (override));
        MOCK_METHOD(void, remove, (std::shared_ptr<Node>, const ID& document), (override));
        MOCK_METHOD(void, disconnect, (const ID& from, const ID& to), (override));
    };
}

TEST(DocumentTests, PropertyAccessorsOK) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);

    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::Radius{7});

    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);

    std::vector<std::shared_ptr<S3D::Edge>> edges = { edge1, edge2 };
    std::vector<std::shared_ptr<S3D::Node>> nodes = { unionNode, sphere1, sphere2 };

    auto doc = S3D::DocumentImpl{docId, interactor, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>()};

    EXPECT_EQ(doc.id(), docId);
    EXPECT_EQ(doc.graph()->edges(), edges);
    EXPECT_EQ(doc.graph()->nodes(), nodes);
}

TEST(DocumentTests, UpdateSphereRadius) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto sphere = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
    std::vector<std::shared_ptr<S3D::Edge>> edges{};
    std::vector<std::shared_ptr<S3D::Node>> nodes = { sphere };

    auto doc = S3D::DocumentImpl(docId, interactor, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    auto new_radius = S3D::Radius{100};

    sphere->radius = new_radius;

    EXPECT_CALL(*interactor, update(sphere->id(), S3D::Attribute{sphere->radius})).Times(1);
    EXPECT_CALL(*interactor, update(sphere->id(), S3D::Attribute{sphere->coord})).Times(1);

    doc.update(sphere);

    EXPECT_EQ(std::dynamic_pointer_cast<S3D::Sphere>(doc.graph()->nodes().at(0))->radius, new_radius);
}

TEST(DocumentTests, UpdateSphereCoords) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto sphere = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
    std::vector<std::shared_ptr<S3D::Edge>> edges{};
    std::vector<std::shared_ptr<S3D::Node>> nodes = { sphere };

    auto doc = S3D::DocumentImpl(docId, interactor, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    auto new_coord = S3D::Coord{100, 200, 300};
    sphere->coord = new_coord;

    EXPECT_CALL(*interactor, update(sphere->id(), S3D::Attribute{sphere->radius})).Times(1);
    EXPECT_CALL(*interactor, update(sphere->id(), S3D::Attribute{sphere->coord})).Times(1);
    doc.update(sphere);

    auto actualCoord = std::dynamic_pointer_cast<S3D::Sphere>(doc.graph()->nodes().at(0))->coord;
    EXPECT_FLOAT_EQ(actualCoord.x, new_coord.x);
    EXPECT_FLOAT_EQ(actualCoord.y, new_coord.y);
    EXPECT_FLOAT_EQ(actualCoord.z, new_coord.z);
}

TEST(DocumentTests, CreateSetOp) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    std::vector<std::shared_ptr<S3D::Edge>> edges{};
    std::vector<std::shared_ptr<S3D::Node>> nodes{};

    auto doc = S3D::DocumentImpl(docId, interactor, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    std::shared_ptr<S3D::Node> setOp = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Intersection);

    EXPECT_CALL(*interactor, create(setOp, docId)).Times(1);

    doc.create(setOp);
}

TEST(DocumentTests, CreateSphere) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    std::vector<std::shared_ptr<S3D::Edge>> edges{};
    std::vector<std::shared_ptr<S3D::Node>> nodes{};

    auto doc = S3D::DocumentImpl(docId, interactor, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    std::shared_ptr<S3D::Node> sphere = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});

    EXPECT_CALL(*interactor, create(sphere, docId)).Times(1);

    doc.create(sphere);
}

TEST(DocumentTests, RemoveSetOp) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    std::shared_ptr<S3D::Node> unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::Radius{7});
    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);

    std::vector<std::shared_ptr<S3D::Edge>> edges = {edge1, edge2};

    std::vector<std::shared_ptr<S3D::Node>> nodes = {unionNode, sphere1, sphere2};

    auto doc = S3D::DocumentImpl(docId, interactor, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    EXPECT_CALL(*interactor, remove(unionNode, docId));

    doc.remove(unionNode);
}

TEST(DocumentTests, RemoveSphere) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
    std::shared_ptr<S3D::Node> sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::Radius{7});
    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);

    std::vector<std::shared_ptr<S3D::Edge>> edges = {edge1, edge2};

    std::vector<std::shared_ptr<S3D::Node>> nodes = {unionNode, sphere1, sphere2};

    auto doc = S3D::DocumentImpl(docId, interactor, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    EXPECT_CALL(*interactor, remove(sphere1, docId));

    doc.remove(sphere1);
}

TEST(DocumentTest, CreateEdge) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::Radius{7});

    std::vector<std::shared_ptr<S3D::Edge>> edges = {};

    std::vector<std::shared_ptr<S3D::Node>> nodes = {unionNode, sphere1, sphere2};

    auto doc = S3D::DocumentImpl(docId, interactor, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    EXPECT_CALL(*interactor, connect(unionNode->id(), sphere1->id())).Times(1);

    doc.create(std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1));
}

TEST(DocumentTest, RemoveEdge) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::Radius{7});
    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);

    std::vector<std::shared_ptr<S3D::Edge>> edges = {edge1, edge2};

    std::vector<std::shared_ptr<S3D::Node>> nodes = {unionNode, sphere1, sphere2};

    auto doc = S3D::DocumentImpl(docId, interactor, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());

    EXPECT_CALL(*interactor, disconnect(unionNode->id(), sphere1->id()));

    doc.remove(edges.at(0));

}
