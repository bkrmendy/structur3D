//
// Created by Berci on 2020. 12. 02..
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <db/Database.h>
#include <wire/Server.h>
#include <wire/ServerController.h>

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

    class MockServer : public Server {
    public:
        MOCK_METHOD(void, sync, (ID& document, Timestamp& timestamp, std::vector<Protocol::Message>& messages), (const, override));
        MOCK_METHOD(void, receive, (Protocol::Message& message), (const, override));
        MOCK_METHOD(void, broadcast, (Protocol::Message& message), (const, override));
    };
}

using namespace S3D::Protocol;
using ::testing::_;

TEST(ServerTests, ReceiveMessageFlow) {
    std::unique_ptr<S3D::MockDatabase> mock_db = std::make_unique<S3D::MockDatabase>();
    std::unique_ptr<S3D::MockServer> mock_server = std::make_unique<S3D::MockServer>();

    EXPECT_CALL(*mock_server, broadcast(_));

    S3D::ServerController server = S3D::ServerController{std::move(mock_db), std::move(mock_server)};

    auto make_id = S3D::IDFactory();
    auto node = std::make_shared<S3D::SetOp>(make_id(), S3D::SetOperationType::Union);

    auto message = Message{CreateDelete{make_id(),
                                        S3D::Protocol::Node{*node},
                                        Change::Retract},
                           1};

    server.receive(message);
}

TEST(ServerTests, SyncClient) {
    FAIL() << "Not implemented";
}

