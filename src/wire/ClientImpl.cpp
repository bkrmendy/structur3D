//
// Created by Berci on 2020. 11. 22..
//

#include <wire/ClientImpl.h>
#include <utils/Overloaded.h>

namespace S3D {
    void ClientImpl::sync(std::vector<Protocol::Message>& message) const {

    }

    void ClientImpl::send(Protocol::Message &message) const {
        // TODO: implement
    }

    void ClientImpl::receive(Protocol::Message &message) const {
        process(message);
    }

    void ClientImpl::connect(std::string &uri) const {
        // TODO: implement
    }

    void ClientImpl::process(Protocol::Message& message) const {
        Protocol::Payload payload = message.payload;
        std::visit(overloaded {
                [interactor = this->interactor_, &message](Protocol::ConnectDisconnect& edge) {
                    switch (edge.deleted) {
                        case Protocol::Change::Assert:
                            return interactor->connect(edge.from, edge.to, message.timestamp);
                        case Protocol::Change::Retract:
                            return interactor->disconnect(edge.from, edge.to, message.timestamp);
                    }
                },
                [this, &message](Protocol::Update& update) {
                    this->interactor_->upsert(update.uid, update.attribute, message.timestamp);
                },
                [this, &message](Protocol::CreateDelete& create) {
                    this->process(create, message.timestamp);
                }
        }, payload);
    }

    void ClientImpl::process(Protocol::CreateDelete &message, Timestamp timestamp) const {
        std::visit(overloaded {
            [this, &message, timestamp](Sphere& sphere) {
                switch (message.create) {
                    case Protocol::Change::Assert:
                        return this->interactor_->create(std::make_shared<Sphere>(sphere), message.document, timestamp);
                    case Protocol::Change::Retract:
                        return this->interactor_->remove(std::make_shared<Sphere>(sphere), message.document, timestamp);
                }
            },
            [this, &message, timestamp](SetOp& setOp) {
                switch (message.create) {
                    case Protocol::Change::Assert:
                        return this->interactor_->create(std::make_shared<SetOp>(setOp), message.document, timestamp);
                    case Protocol::Change::Retract:
                        return this->interactor_->remove(std::make_shared<SetOp>(setOp), message.document, timestamp);
                }
            }
        }, message.node);
    }
}
