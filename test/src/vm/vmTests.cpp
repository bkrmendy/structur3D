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
    class MockDatabase : public S3D::Database {
    public:
        MOCK_METHOD(void, upsert, (const ID&, const RADIUS&, Timestamp timestamp), (override));
        MOCK_METHOD(void, retract, (const ID&, const RADIUS&, Timestamp timestamp), (override));

        MOCK_METHOD(void, upsert, (const ID&, const Coord&, Timestamp timestamp), (override));
        MOCK_METHOD(void, retract, (const ID&, const Coord&, Timestamp timestamp), (override));

        MOCK_METHOD(void, upsert, (const ID&, const SetOperationType&, Timestamp timestamp), (override));
        MOCK_METHOD(void, retract, (const ID&, const SetOperationType&, Timestamp timestamp), (override));

        MOCK_METHOD(void, upsert, (const ID&, const std::string& name, Timestamp timestamp), (override));
        MOCK_METHOD(void, retract, (const ID&, const std::string& name, Timestamp timestamp), (override));

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
}

using ::testing::Return;
using ::testing::WithArg;
using ::testing::WithArgs;

TEST(ViewModelTests, BrandNewViewModel) {
    std::shared_ptr<S3D::MockDatabase> mockDB = std::make_shared<S3D::MockDatabase>();
    auto makeID = S3D::IDFactory();

    std::vector<S3D::DocumentWithName> docs = {{ makeID(), "Doc 1" }, { makeID(), "Doc 2" }};

    ON_CALL(*mockDB, documents())
            .WillByDefault(Return(docs));

    auto vm = S3D::ViewModelImpl{mockDB};

    EXPECT_EQ(vm.document(), std::nullopt);
    EXPECT_TRUE(vm.message().empty());
    EXPECT_EQ(vm.documents().size(), 2);
    EXPECT_EQ(vm.documents().at(0).uid, docs.at(0).uid);
    EXPECT_EQ(vm.documents().at(0).name, docs.at(0).name);
    EXPECT_EQ(vm.documents().at(1).uid, docs.at(1).uid);
    EXPECT_EQ(vm.documents().at(1).name, docs.at(1).name);
}

TEST(ViewModelTests, CreateDocument) {
    std::shared_ptr<S3D::MockDatabase> mockDB = std::make_shared<S3D::MockDatabase>();
    auto makeID = S3D::IDFactory();

    std::vector<S3D::DocumentWithName> docs{};

    ON_CALL(*mockDB, documents())
            .WillByDefault(Return(docs));

    auto vm = S3D::ViewModelImpl{mockDB};

    EXPECT_TRUE(vm.documents().empty());

    auto name = "Test Document";

    vm.createDocument(name);
    EXPECT_EQ(vm.documents().size(), 1);
    EXPECT_EQ(vm.documents().at(0).name, name);
}

TEST(ViewModelTests, OpenDocument) {
    std::shared_ptr<S3D::MockDatabase> mockDB = std::make_shared<S3D::MockDatabase>();
    auto makeID = S3D::IDFactory();

    auto dummyDocId = makeID();
    auto unionID = makeID();
    auto unionNode = S3D::SetOp{unionID, S3D::SetOperationType::Union};

    auto s1ID = makeID();
    // NOLINTNEXTLINE readability-magic-numbers
    auto s1 = S3D::Sphere{s1ID, S3D::Coord{1,2,3}, S3D::RADIUS{6}};

    auto s2ID = makeID();
    // NOLINTNEXTLINE readability-magic-numbers
    auto s2 = S3D::Sphere{s2ID, S3D::Coord{4,5,6}, S3D::RADIUS{9}};

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

    auto vm = S3D::ViewModelImpl{mockDB};

    vm.open(dummyDocId);
    EXPECT_TRUE(vm.document().has_value());

    auto& currentDoc = vm.currentDocument_.value();
    EXPECT_EQ(currentDoc->id(), dummyDocId);
    EXPECT_EQ(currentDoc->graph()->edges.size(), 2);
    EXPECT_EQ(currentDoc->graph()->nodes.size(), 3);
    EXPECT_EQ(currentDoc->graph()->edges.at(0)->from->id(), unionID);
    EXPECT_EQ(currentDoc->graph()->edges.at(1)->from->id(), unionID);
}
