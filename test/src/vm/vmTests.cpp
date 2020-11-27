//
// Created by Berci on 2020. 11. 19..
//

#include <memory>
#include <vector>
#include <optional>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "data/Radius.h"

#include "mesh/MeshFactory.h"

#include "db/Database.h"
#include "vm/ViewmodelImpl.h"

namespace S3D {
    class MockDatabase : public Database {
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

    class MockNetwork : public ClientEndpoint {
    public:
        MOCK_METHOD(void, connect, (std::string& uri), (const, override));
        MOCK_METHOD(void, sync, (std::vector<Protocol::Message>&), (const, override));
        MOCK_METHOD(void, send, (Protocol::Message&), (const, override));
        MOCK_METHOD(void, receive, (Protocol::Message&), (const, override));
    };

    class MockMeshFactory : public MeshFactory {
        std::unique_ptr<Mesh> fromTree(const std::vector<Tree>&) {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
            return std::make_unique<Mesh>(vertices, indices);
        }
    };
}

using ::testing::Return;
using ::testing::_;

using namespace S3D::Protocol;

TEST(ViewModelTests, BrandNewViewModel) {
    std::unique_ptr<S3D::MockDatabase> mockDB = std::make_unique<S3D::MockDatabase>();
    std::unique_ptr<S3D::MockNetwork> mockNet = std::make_unique<S3D::MockNetwork>();
    auto makeID = S3D::IDFactory();

    std::vector<S3D::DocumentWithName> docs = {{ makeID(), S3D::Name{"Doc 1"} }, { makeID(), S3D::Name{"Doc 2"} }};

    ON_CALL(*mockDB, documents())
            .WillByDefault(Return(docs));

    auto vm = std::make_shared<S3D::ViewModelImpl>(std::move(mockDB), std::move(mockNet));

    EXPECT_EQ(vm->document(), nullptr);
    EXPECT_TRUE(vm->message().empty());
    EXPECT_EQ(vm->documents().size(), 2);
    EXPECT_EQ(vm->documents().at(0).uid, docs.at(0).uid);
    EXPECT_EQ(vm->documents().at(0).name, docs.at(0).name);
    EXPECT_EQ(vm->documents().at(1).uid, docs.at(1).uid);
    EXPECT_EQ(vm->documents().at(1).name, docs.at(1).name);
}

TEST(ViewModelTests, CreateDocument) {
    std::unique_ptr<S3D::MockDatabase> mockDB = std::make_unique<S3D::MockDatabase>();
    std::unique_ptr<S3D::ClientEndpoint> mockNet = std::make_unique<S3D::MockNetwork>();
    auto makeID = S3D::IDFactory();

    std::vector<S3D::DocumentWithName> docs{};

    ON_CALL(*mockDB, documents())
            .WillByDefault(Return(docs));

    auto name = S3D::Name{"Test Document"};
    EXPECT_CALL(*mockDB, upsert(_, name, _));

    auto vm = std::make_shared<S3D::ViewModelImpl>(std::move(mockDB), std::move(mockNet));

    EXPECT_TRUE(vm->documents().empty());

    vm->createDocument(name);
    EXPECT_EQ(vm->documents().size(), 1);
    EXPECT_EQ(vm->documents().at(0).name, name);
}

TEST(ViewModelTests, OpenDocument) {
    std::unique_ptr<S3D::MockDatabase> mockDB = std::make_unique<S3D::MockDatabase>();
    std::unique_ptr<S3D::MockNetwork> mockNet = std::make_unique<S3D::MockNetwork>();
    auto makeID = S3D::IDFactory();

    auto dummyDocId = makeID();
    auto unionID = makeID();
    auto unionNode = S3D::SetOp{unionID, S3D::SetOperationType::Union};

    auto s1ID = makeID();
    // NOLINTNEXTLINE readability-magic-numbers
    auto s1 = S3D::Sphere{s1ID, S3D::Coord{1,2,3}, S3D::Radius{6}};

    auto s2ID = makeID();
    // NOLINTNEXTLINE readability-magic-numbers
    auto s2 = S3D::Sphere{s2ID, S3D::Coord{4,5,6}, S3D::Radius{9}};

    auto dummyIDs = std::vector<S3D::IDWithType>{
        { unionID, S3D::NodeType::SetOperation }, { s1ID, S3D::NodeType::Sphere }, { s2ID, S3D::NodeType::Sphere }
    };

    ON_CALL(*mockDB, entities(dummyDocId))
        .WillByDefault(Return(dummyIDs));
    ON_CALL(*mockDB, setop(unionID))
        .WillByDefault(Return(unionNode));
    ON_CALL(*mockDB, sphere(s1ID))
        .WillByDefault(Return(s1));
    ON_CALL(*mockDB, sphere(s2ID))
        .WillByDefault(Return(s2));
    ON_CALL(*mockDB, edges(unionID))
        .WillByDefault(Return(std::vector<S3D::ID>{s1ID, s2ID}));

    ON_CALL(*mockNet, send(_)).WillByDefault(Return());

    auto vm = std::make_shared<S3D::ViewModelImpl>(std::move(mockDB), std::move(mockNet));

    vm->open(dummyDocId);
    EXPECT_TRUE(vm->document() != nullptr);

    const auto& currentDoc = vm->document();
    EXPECT_EQ(currentDoc->id(), dummyDocId);
    EXPECT_EQ(currentDoc->graph()->edges().size(), 2);
    EXPECT_EQ(currentDoc->graph()->nodes().size(), 3);
    EXPECT_EQ(currentDoc->graph()->edges().at(0)->from->id(), unionID);
    EXPECT_EQ(currentDoc->graph()->edges().at(1)->from->id(), unionID);

    vm->db_.reset();
    vm->network_.reset();
}

TEST(ViewModelTests, PropertyAccessorsOK) {
    std::unique_ptr<S3D::MockDatabase> mockDB = std::make_unique<S3D::MockDatabase>();
    std::unique_ptr<S3D::MockNetwork> mockNet = std::make_unique<S3D::MockNetwork>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);

    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::Radius{7});

    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);

    std::vector<std::shared_ptr<S3D::Edge>> edges = { edge1, edge2 };
    std::vector<std::shared_ptr<S3D::Node>> nodes = { unionNode, sphere1, sphere2 };

    ON_CALL(*mockDB, entities(docId))
        .WillByDefault(
                Return(std::vector<S3D::IDWithType>{
                        {sphere1->id(), S3D::NodeType::Sphere},
                        {sphere2->id(), S3D::NodeType::Sphere},
                        {unionNode->id(), S3D::NodeType::SetOperation}}));

    ON_CALL(*mockDB, sphere(sphere1->id()))
        .WillByDefault(Return(std::optional{*sphere1}));
    ON_CALL(*mockDB, sphere(sphere2->id()))
            .WillByDefault(Return(std::optional{*sphere2}));
    ON_CALL(*mockDB, setop(unionNode->id()))
            .WillByDefault(Return(std::optional{*unionNode}));
    ON_CALL(*mockDB, edges(unionNode->id()))
            .WillByDefault(
                    Return(std::vector<S3D::ID>{sphere1->id(), sphere2->id()}));

    EXPECT_CALL(*mockDB, entities(docId));
    EXPECT_CALL(*mockDB, sphere(sphere1->id()));
    EXPECT_CALL(*mockDB, sphere(sphere2->id()));
    EXPECT_CALL(*mockDB, setop(unionNode->id()));
    EXPECT_CALL(*mockDB, edges(unionNode->id()));
    EXPECT_CALL(*mockDB, edges(sphere1->id()));
    EXPECT_CALL(*mockDB, edges(sphere2->id()));

    auto vm = std::make_shared<S3D::ViewModelImpl>(std::move(mockDB), std::move(mockNet));

    vm->open(docId);

    EXPECT_EQ(vm->document()->id(), docId);
    EXPECT_EQ(vm->document()->graph()->edges().size(), 2);
    EXPECT_EQ(vm->document()->graph()->nodes().size(), 3);
    EXPECT_EQ(vm->document()->graph()->roots().size(), 1);
    EXPECT_EQ(vm->document()->graph()->roots().at(0)->id(), unionNode->id());

    vm->db_.reset();
    vm->network_.reset();
}

TEST(ViewModelTests, UpdateSphereRadius) {
    std::unique_ptr<S3D::MockDatabase> mockDB = std::make_unique<S3D::MockDatabase>();
    std::unique_ptr<S3D::MockNetwork> mockNet = std::make_unique<S3D::MockNetwork>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);

    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::Radius{7});

    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);

    std::vector<std::shared_ptr<S3D::Edge>> edges = { edge1, edge2 };
    std::vector<std::shared_ptr<S3D::Node>> nodes = { unionNode, sphere1, sphere2 };

    ON_CALL(*mockDB, entities(docId))
            .WillByDefault(
                    Return(std::vector<S3D::IDWithType>{
                            {sphere1->id(), S3D::NodeType::Sphere},
                            {sphere2->id(), S3D::NodeType::Sphere},
                            {unionNode->id(), S3D::NodeType::SetOperation}}));

    ON_CALL(*mockDB, sphere(sphere1->id()))
            .WillByDefault(Return(std::optional{*sphere1}));
    ON_CALL(*mockDB, sphere(sphere2->id()))
            .WillByDefault(Return(std::optional{*sphere2}));
    ON_CALL(*mockDB, setop(unionNode->id()))
            .WillByDefault(Return(std::optional{*unionNode}));
    ON_CALL(*mockDB, edges(unionNode->id()))
            .WillByDefault(
                    Return(std::vector<S3D::ID>{sphere1->id(), sphere2->id()}));

    const auto new_radius = S3D::Radius{111};

    EXPECT_CALL(*mockDB, upsert(sphere1->id(), new_radius, _));
    EXPECT_CALL(*mockDB, upsert(sphere1->id(), sphere1->coord, _));
    EXPECT_CALL(*mockNet, send(_)).Times(2);

    auto vm = std::make_shared<S3D::ViewModelImpl>(std::move(mockDB), std::move(mockNet));

    vm->open(docId);

    const auto& doc = vm->document();

    sphere1->radius = new_radius;

    doc->update(sphere1);

    auto actualRadius = std::dynamic_pointer_cast<S3D::Sphere>(doc->graph()->nodes().at(0))->radius;
    EXPECT_EQ(actualRadius, new_radius);

    vm->db_.reset();
    vm->network_.reset();
}

TEST(ViewModelTests, UpdateSphereCoords) {
    std::unique_ptr<S3D::MockDatabase> mockDB = std::make_unique<S3D::MockDatabase>();
    std::unique_ptr<S3D::MockNetwork> mockNet = std::make_unique<S3D::MockNetwork>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);

    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::Radius{7});

    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);

    std::vector<std::shared_ptr<S3D::Edge>> edges = { edge1, edge2 };
    std::vector<std::shared_ptr<S3D::Node>> nodes = { unionNode, sphere1, sphere2 };

    ON_CALL(*mockDB, entities(docId))
            .WillByDefault(
                    Return(std::vector<S3D::IDWithType>{
                            {sphere1->id(), S3D::NodeType::Sphere},
                            {sphere2->id(), S3D::NodeType::Sphere},
                            {unionNode->id(), S3D::NodeType::SetOperation}}));

    ON_CALL(*mockDB, sphere(sphere1->id()))
            .WillByDefault(Return(std::optional{*sphere1}));
    ON_CALL(*mockDB, sphere(sphere2->id()))
            .WillByDefault(Return(std::optional{*sphere2}));
    ON_CALL(*mockDB, setop(unionNode->id()))
            .WillByDefault(Return(std::optional{*unionNode}));
    ON_CALL(*mockDB, edges(unionNode->id()))
            .WillByDefault(
                    Return(std::vector<S3D::ID>{sphere1->id(), sphere2->id()}));

    auto new_coord = S3D::Coord{100, 200, 300};

    EXPECT_CALL(*mockDB, upsert(sphere1->id(), new_coord, _));
    EXPECT_CALL(*mockDB, upsert(sphere1->id(), sphere1->radius, _));
    EXPECT_CALL(*mockNet, send(_)).Times(2);

    auto vm = std::make_shared<S3D::ViewModelImpl>(std::move(mockDB), std::move(mockNet));

    vm->open(docId);

    const auto& doc = vm->document();

    sphere1->coord = new_coord;

    doc->update(sphere1);

    auto actual_coords = std::dynamic_pointer_cast<S3D::Sphere>(doc->graph()->nodes().at(0))->coord;
    EXPECT_EQ(actual_coords, new_coord);

    vm->db_.reset();
    vm->network_.reset();
}

TEST(ViewModelTests, CreateSetOp) {
    std::unique_ptr<S3D::MockDatabase> mockDB = std::make_unique<S3D::MockDatabase>();
    std::unique_ptr<S3D::MockNetwork> mockNet = std::make_unique<S3D::MockNetwork>();
    auto makeId = S3D::IDFactory();
    const auto docId = makeId();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);

    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::Radius{7});

    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);

    std::vector<std::shared_ptr<S3D::Edge>> edges = { edge1, edge2 };
    std::vector<std::shared_ptr<S3D::Node>> nodes = { unionNode, sphere1, sphere2 };

    ON_CALL(*mockDB, entities(docId))
            .WillByDefault(
                    Return(std::vector<S3D::IDWithType>{
                            {sphere1->id(), S3D::NodeType::Sphere},
                            {sphere2->id(), S3D::NodeType::Sphere},
                            {unionNode->id(), S3D::NodeType::SetOperation}}));

    ON_CALL(*mockDB, sphere(sphere1->id()))
            .WillByDefault(Return(std::optional{*sphere1}));
    ON_CALL(*mockDB, sphere(sphere2->id()))
            .WillByDefault(Return(std::optional{*sphere2}));
    ON_CALL(*mockDB, setop(unionNode->id()))
            .WillByDefault(Return(std::optional{*unionNode}));
    ON_CALL(*mockDB, edges(unionNode->id()))
            .WillByDefault(
                    Return(std::vector<S3D::ID>{sphere1->id(), sphere2->id()}));

    auto node_id = makeId();

    EXPECT_CALL(*mockDB, create(node_id, S3D::NodeType::Sphere, docId, _));
    EXPECT_CALL(*mockDB, upsert(node_id, S3D::SetOperationType::Union, _));
    EXPECT_CALL(*mockNet, send(_));

    auto vm = std::make_shared<S3D::ViewModelImpl>(std::move(mockDB), std::move(mockNet));
    vm->open(docId);
    const auto& doc = vm->document();

    doc->create(std::make_shared<S3D::SetOp>(node_id, S3D::SetOperationType::Union));

    EXPECT_EQ(doc->graph()->nodes().size(), 4);
    EXPECT_EQ(doc->graph()->edges().size(), 2);

    vm->db_.reset();
    vm->network_.reset();
}

//TEST(ViewModelTests, CreateSphere) {
//    std::unique_ptr<S3D::MockDatabase> mockDB = std::make_unique<S3D::MockDatabase>();
//    std::unique_ptr<S3D::MockNetwork> mockNet = std::make_unique<S3D::MockNetwork>();
//    auto makeId = S3D::IDFactory();
//    const auto docId = makeId();
//    std::vector<std::shared_ptr<S3D::Edge>> edges{};
//    std::vector<std::shared_ptr<S3D::Node>> nodes{};
//
//    auto vm = std::make_shared<S3D::ViewModelImpl>(std::move(mockDB), std::move(mockNet));
//    vm->open(docId);
//
//    const auto& doc = vm->document();
//
//    std::shared_ptr<S3D::Node> sphere = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
//
//    EXPECT_CALL(*interactor, create(sphere, docId)).Times(1);
//
//    doc->create(sphere);
//}

/*
 * 11. 27: deferred for later
 */

//TEST(ViewModelTests, RemoveSetOp) {
//    std::unique_ptr<S3D::MockDatabase> mockDB = std::make_unique<S3D::MockDatabase>();
//    std::unique_ptr<S3D::MockNetwork> mockNet = std::make_unique<S3D::MockNetwork>();
//    auto makeId = S3D::IDFactory();
//    const auto docId = makeId();
//    std::shared_ptr<S3D::Node> unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
//    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
//    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::Radius{7});
//    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
//    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);
//
//    std::vector<std::shared_ptr<S3D::Edge>> edges = {edge1, edge2};
//
//    std::vector<std::shared_ptr<S3D::Node>> nodes = {unionNode, sphere1, sphere2};
//
//    auto vm = std::make_shared<S3D::ViewModelImpl>(std::move(mockDB), std::move(mockNet));
//    vm->open(docId);
//
//    const auto& doc = vm->document();
//
//    EXPECT_CALL(*interactor, remove(unionNode, docId));
//
//    doc->remove(unionNode);
//}

//TEST(ViewModelTests, RemoveSphere) {
//    std::unique_ptr<S3D::MockDatabase> mockDB = std::make_unique<S3D::MockDatabase>();
//    std::unique_ptr<S3D::MockNetwork> mockNet = std::make_unique<S3D::MockNetwork>();
//    auto makeId = S3D::IDFactory();
//    const auto docId = makeId();
//    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
//    std::shared_ptr<S3D::Node> sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
//    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::Radius{7});
//    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
//    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);
//
//    std::vector<std::shared_ptr<S3D::Edge>> edges = {edge1, edge2};
//
//    std::vector<std::shared_ptr<S3D::Node>> nodes = {unionNode, sphere1, sphere2};
//
//    auto vm = std::make_shared<S3D::ViewModelImpl>(std::move(mockDB), std::move(mockNet));
//    vm->open(docId);
//
//    const auto& doc = vm->document();
//
//    EXPECT_CALL(*interactor, remove(sphere1, docId));
//
//    doc->remove(sphere1);
//}

//TEST(ViewModelTests, CreateEdge) {
//    std::unique_ptr<S3D::MockDatabase> mockDB = std::make_unique<S3D::MockDatabase>();
//    std::unique_ptr<S3D::MockNetwork> mockNet = std::make_unique<S3D::MockNetwork>();
//    auto makeId = S3D::IDFactory();
//    const auto docId = makeId();
//    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
//    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
//    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::Radius{7});
//
//    std::vector<std::shared_ptr<S3D::Edge>> edges = {};
//
//    std::vector<std::shared_ptr<S3D::Node>> nodes = {unionNode, sphere1, sphere2};
//
//    auto vm = std::make_shared<S3D::ViewModelImpl>(std::move(mockDB), std::move(mockNet));
//    vm->open(docId);
//
//    const auto& doc = vm->document();
//
//    EXPECT_CALL(*interactor, connect(unionNode->id(), sphere1->id())).Times(1);
//
//    doc->create(std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1));
//}

//TEST(ViewModelTests, RemoveEdge) {
//    std::unique_ptr<S3D::MockDatabase> mockDB = std::make_unique<S3D::MockDatabase>();
//    std::unique_ptr<S3D::MockNetwork> mockNet = std::make_unique<S3D::MockNetwork>();
//    auto makeId = S3D::IDFactory();
//    const auto docId = makeId();
//    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
//    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::Radius{4});
//    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::Radius{7});
//    auto edge1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
//    auto edge2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);
//
//    std::vector<std::shared_ptr<S3D::Edge>> edges = {edge1, edge2};
//
//    std::vector<std::shared_ptr<S3D::Node>> nodes = {unionNode, sphere1, sphere2};
//
//    auto doc = S3D::DocumentImpl(docId, interactor, std::make_unique<S3D::Graph>(edges, nodes), std::make_unique<S3D::MockMeshFactory>());
//
//    EXPECT_CALL(*interactor, disconnect(unionNode->id(), sphere1->id()));
//
//    doc.remove(edges.at(0));
//}
