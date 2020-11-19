//
// Created by Berci on 2020. 11. 19..
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

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

TEST(DocumentTests, UpdateSphereRadius) {
    FAIL() << "Not implemented";
}

TEST(DocumentTests, UpdateSphereCoords) {
    FAIL() << "Not implemented";
}

TEST(DocumentTests, RemoveSetOp) {
    FAIL() << "Not implemented";
}

TEST(DocumentTests, RemoveSphere) {
    FAIL() << "Not implemented";
}

TEST(DocumentTests, CreateSetOp) {
    FAIL() << "Not implemented";
}

TEST(DocumentTests, CreateSphere) {
    FAIL() << "Not implemented";
}

TEST(DocumentTests, TreesCorrect) {

}

TEST(DocumentTests, GraphCorrect) {

}

