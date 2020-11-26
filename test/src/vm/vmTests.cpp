//
// Created by Berci on 2020. 11. 19..
//

#include <memory>
#include <vector>
#include <optional>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

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
}

using ::testing::Return;
using ::testing::_;

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
