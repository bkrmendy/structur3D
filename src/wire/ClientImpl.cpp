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
                [interactor = this->interactor_, &message](Protocol::Edge& edge) {
                    if (message.deleted) {
                        interactor->disconnect(edge.from, edge.to, message.timestamp);
                    } else {
                        interactor->connect(edge.from, edge.to, message.timestamp);
                    }
                },
                [this, &message](Protocol::Update& update) {
                     this->process(update, message.deleted, message.timestamp);
                },
                [this, &message](Protocol::Create& create) {
                    this->process(create, message.deleted, message.timestamp);
                }
        }, payload);
    }

    void ClientImpl::process(Protocol::Update &update, bool deleted, Timestamp timestamp) const {
        if(deleted) {
            this->interactor_->retract(update.uid, update.attribute, timestamp);
        } else {
            this->interactor_->upsert(update.uid, update.attribute, timestamp);
        }
    }

    void ClientImpl::process(Protocol::Create &create, bool deleted, Timestamp timestamp) const {
        std::visit(overloaded {
            [this, doc = create.document, deleted, timestamp](Sphere& sphere) {
                if (deleted) {
                    this->interactor_->remove(std::make_shared<Sphere>(sphere), doc, timestamp);
                } else {
                    this->interactor_->create(std::make_shared<Sphere>(sphere), doc, timestamp);
                }
            },
            [this, doc = create.document, deleted, timestamp](SetOp& setOp) {
                if (deleted) {
                    this->interactor_->remove(std::make_shared<SetOp>(setOp), doc, timestamp);
                } else {
                    this->interactor_->create(std::make_shared<SetOp>(setOp), doc, timestamp);
                }
            }
        }, create.node);
    }
}
