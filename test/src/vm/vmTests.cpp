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

        MOCK_METHOD(void, create, (const ID& entity, const NodeType type, const ID& document), (override));
        MOCK_METHOD(void, remove, (const ID& entity, const ID& document), (override));

        MOCK_METHOD(std::vector<DocumentWithName>, documents, (), (override));
        MOCK_METHOD(std::vector<IDWithType>, entities, (const ID&), (override));
        MOCK_METHOD(std::optional<Sphere>, sphere, (const ID&), (override));
        MOCK_METHOD(std::optional<SetOp>, setop, (const ID& from), (override));
        MOCK_METHOD(std::vector<ID>, edges, (const ID&), (override));
    };
}

using ::testing::Return;

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
