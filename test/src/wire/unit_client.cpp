//
// Created by Berci on 2020. 11. 27..
//


#include <memory>
#include <wire/ClientImpl.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "data/Edge.h"
#include "db/Database.h"
#include "data/Attribute.h"


namespace S3D {
    class MockInteractor : public Interactor {
    public:
        MOCK_METHOD(void, create, (std::shared_ptr<Node>, const ID& document, Timestamp timestamp), (override));
        MOCK_METHOD(void, connect, (const ID& from, const ID& to, Timestamp timestamp), (override));
        MOCK_METHOD(void, upsert, (const ID& entity, const Attribute& attribute, Timestamp timestamp), (override));
        MOCK_METHOD(void, remove, (std::shared_ptr<Node>, const ID& document, Timestamp timestamp), (override));
        MOCK_METHOD(void, disconnect, (const ID& from, const ID& to, Timestamp timestamp), (override));
    };
}

using namespace S3D::Protocol;
using ::testing::_;

TEST(ClientImplTests, CreateMessage) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    S3D::ClientImpl client{interactor};
    auto make_id = S3D::IDFactory();

    auto node_id = make_id();
    auto doc_id = make_id();

    auto node = S3D::SetOp{node_id, S3D::SetOperationType::Union};

    auto nodePtr = std::make_shared<S3D::SetOp>(node);

    EXPECT_CALL(*interactor, create(_, doc_id, 1));

    auto message = Message{CreateDelete{doc_id,
                                        S3D::Protocol::Node{node},
                                        Change::Assert},
                           1};
    client.receive(message);
}

TEST(ClientImplTests, RemoveMessage) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    S3D::ClientImpl client{interactor};
    auto make_id = S3D::IDFactory();

    auto node_id = make_id();
    auto doc_id = make_id();

    auto node = std::make_shared<S3D::SetOp>(node_id, S3D::SetOperationType::Union);

    EXPECT_CALL(*interactor, remove(_, doc_id, 1));

    auto message = Message{CreateDelete{doc_id,
                                        S3D::Protocol::Node{*node},
                                        Change::Retract},
                           1};
    client.receive(message);
}

TEST(ClientImplTests, ConnectMessage) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    S3D::ClientImpl client{interactor};
    auto make_id = S3D::IDFactory();

    auto id_1 = make_id();
    auto id_2 = make_id();

    EXPECT_CALL(*interactor, connect(id_1, id_2, 1));

    auto message = Message{ConnectDisconnect{id_1,
                                             id_2,
                                             Change::Assert},
                           1};
    client.receive(message);
}

TEST(ClientImplTests, DisconnectMessage) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    S3D::ClientImpl client{interactor};
    auto make_id = S3D::IDFactory();

    auto id_1 = make_id();
    auto id_2 = make_id();

    EXPECT_CALL(*interactor, disconnect(id_1, id_2, 1));

    auto message = Message{ConnectDisconnect{id_1,
                                             id_2,
                                             Change::Retract},
                           1};
    client.receive(message);
}

TEST(ClientImplTests, UpdateCoord) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    S3D::ClientImpl client{interactor};

    auto make_id = S3D::IDFactory();
    auto node_id = make_id();

    auto coord = S3D::Coord{1,2,3};

    EXPECT_CALL(*interactor, upsert(node_id, S3D::Attribute{coord}, 1));

    auto message = Message{Update{node_id,
                                    S3D::Protocol::Attribute{coord}},
                           1};

    client.receive(message);
}

TEST(ClientImplTests, UpdateRadius) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    S3D::ClientImpl client{interactor};

    auto make_id = S3D::IDFactory();
    auto node_id = make_id();

    auto radius = S3D::Radius{10};

    EXPECT_CALL(*interactor, upsert(node_id, S3D::Attribute{radius}, 1));

    auto message = Message{Update{node_id,
                                  S3D::Protocol::Attribute{radius}},
                           1};

    client.receive(message);
}

TEST(ClientImplTests, UpdateSetopType) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    S3D::ClientImpl client{interactor};

    auto make_id = S3D::IDFactory();
    auto node_id = make_id();

    auto setOpType = S3D::SetOperationType::Union;

    EXPECT_CALL(*interactor, upsert(node_id, S3D::Attribute{setOpType}, 1));

    auto message = Message{Update{node_id,
                                  S3D::Protocol::Attribute{setOpType}},
                           1};

    client.receive(message);
}

TEST(ClientImplTests, UpdateName) {
    std::shared_ptr<S3D::MockInteractor> interactor = std::make_shared<S3D::MockInteractor>();
    S3D::ClientImpl client{interactor};

    auto make_id = S3D::IDFactory();
    auto node_id = make_id();

    auto name = S3D::Name{"Tutankhamun"};

    EXPECT_CALL(*interactor, upsert(node_id, S3D::Attribute{name}, 1));

    auto message = Message{Update{node_id,
                                  S3D::Protocol::Attribute{name}},
                           1};

    client.receive(message);
}