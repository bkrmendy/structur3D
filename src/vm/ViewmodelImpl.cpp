//
//  VIewmodel.cpp
//  cross-platform-game iOS
//
//  Created by Berci on 2020. 11. 04..
//  Copyright © 2020. Berci. All rights reserved.
//

#include <algorithm>
#include <utility>
#include <utils/Overloaded.h>


#include "vm/ViewmodelImpl.h"
#include "document/DocumentImpl.h"

namespace S3D {
    ViewModelImpl::ViewModelImpl(std::unique_ptr<Database> db, std::unique_ptr<ClientEndpoint> network)
        : db_{std::move(db)}
        , network_{std::move(network)}
        , cancellables_{}
        , documents_{}
        , message_{} {
        this->currentDocument_ = nullptr;
        this->documents_ = this->db_->documents();
    }

    void ViewModelImpl::createDocument(const Name& name) {
        auto makeID = IDFactory();
        auto now = TimestampFactory().timestamp();
        this->db_->upsert(makeID(), name, now);
        this->documents_.emplace_back(makeID(), name);
    }

    const std::vector<DocumentWithName>& ViewModelImpl::documents() const {
        return this->documents_;
    }

    void ViewModelImpl::open(const ID &document) {
    std::vector<std::shared_ptr<Node>> nodes{};
    std::vector<std::shared_ptr<Edge>> edges{};

    auto entities = this->db_->entities(document);

    for (const auto& entity : entities) {
        if (entity.type == NodeType::Sphere) {
            auto maybeSphere = this->db_->sphere(entity.uid);
            if (maybeSphere.has_value()) {
                nodes.push_back(std::make_shared<Sphere>(maybeSphere.value()));
            }
        } else if (entity.type == NodeType::SetOperation) {
            auto maybeSetOp = this->db_->setop(entity.uid);
            if (maybeSetOp.has_value()) {
                nodes.push_back(std::make_shared<SetOp>(maybeSetOp.value()));
            }
        }
    }

    auto makeID = IDFactory();

    for (const auto& node : nodes) {
        for (const auto& toPtr : this->db_->edges(node->id())) {
            for (const auto& toNode : nodes) {
                if (toNode->id() == toPtr) {
                    const auto id = makeID();
                    edges.emplace_back(std::make_shared<Edge>(id, node, toNode));
                }
            }
        }
    }

    this->currentDocument_
        = std::make_unique<DocumentImpl>(document,
                                         shared_from_this(),
                                         std::make_unique<Graph>(edges, nodes),
                                         std::make_unique<MeshFactory>());
        }

    const std::unique_ptr<Document>& ViewModelImpl::document() const {
        return this->currentDocument_;
    }

    const std::string& ViewModelImpl::message() const {
        return this->message_;
    }

    void ViewModelImpl::create(const std::shared_ptr<Node> node, const ID& document, Timestamp now) {
        if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
            this->db_->create(node->id(), NodeType::Sphere, document, now);
            this->db_->upsert(sphere->uid, sphere->radius, now);
            this->db_->upsert(sphere->uid, sphere->coord, now);

            auto message = Protocol::Message{Protocol::Payload{Protocol::CreateDelete{document,
                                                                                      Protocol::Node{*sphere},
                                                                                      Protocol::Change::Assert
                                            }}, now};
            this->network_->send(message);

        } else if (auto setOp = std::dynamic_pointer_cast<SetOp>(node)) {
            this->db_->create(node->id(), NodeType::SetOperation, document, now);
            this->db_->upsert(setOp->uid, setOp->type, now);

            auto message = Protocol::Message{Protocol::Payload{Protocol::CreateDelete{document,
                                                                                      Protocol::Node{*setOp},
                                                                                      Protocol::Change::Assert
            }}, now};
            this->network_->send(message);

        }

        this->document()->graph()->create(node);
    }

    void ViewModelImpl::connect(const ID &from, const ID &to, Timestamp now) {
        this->db_->connect(from, to, now);
        auto message = Protocol::Message{Protocol::Payload{Protocol::ConnectDisconnect{from,
                                                                                       to,
                                                                                       Protocol::Change::Assert}}, now};
        this->network_->send(message);
    }

    void ViewModelImpl::upsert(const ID& entity, const Attribute& attribute, Timestamp now) {
        std::visit(overloaded {
            [this, &now, &entity](const Coord& coord) {
                this->db_->upsert(entity, coord, now);
                auto message = Protocol::Message{Protocol::Payload{Protocol::Update{entity,
                                                                                        Protocol::Attribute{coord}}},
                                                now};
                this->network_->send(message);
                this->document()->graph()->access(entity, [&coord](auto node) {
                    std::dynamic_pointer_cast<Sphere>(node)->coord = coord;
                });
            },
            [this, &now, &entity](const Radius& radius) {
                this->db_->upsert(entity, radius, now);
                auto message = Protocol::Message{Protocol::Payload{Protocol::Update{entity,
                                                                                                Protocol::Attribute{radius}}},
                                                 now};
                this->network_->send(message);
                this->document()->graph()->access(entity, [&radius](auto node) {
                    std::dynamic_pointer_cast<Sphere>(node)->radius = radius;
                });
            },
            [this, &now, &entity](const SetOperationType& type) {
                this->db_->upsert(entity, type, now);
                auto message = Protocol::Message{Protocol::Payload{Protocol::Update{entity,
                                                                                                    Protocol::Attribute{type}}},
                                                 now};
                this->network_->send(message);
                this->document()->graph()->access(entity, [type](auto node) {
                    std::dynamic_pointer_cast<SetOp>(node)->type = type;
                });
            },
            [this, &now, &entity](const Name& name) {
                this->db_->upsert(entity, name, now);
                auto message = Protocol::Message{Protocol::Payload{Protocol::Update{entity,
                                                                                        Protocol::Attribute{name}}},
                                                 now};
                this->network_->send(message);
                for (auto& document : documents_) {
                    if (document.uid == entity) {
                        document.name = name;
                    }
                }
            }
        }, attribute);
    }

    void ViewModelImpl::remove(std::shared_ptr<Node> node, const ID &document, Timestamp now) {
        this->db_->remove(node->id(), document, now);
        if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
            this->db_->retract(sphere->uid, sphere->radius, now);
            this->db_->retract(sphere->uid, sphere->coord, now);
        } else if (auto setOp = std::dynamic_pointer_cast<SetOp>(node)) {
            this->db_->retract(setOp->uid, setOp->type, now);
        }
    }

    void ViewModelImpl::disconnect(const ID &from, const ID &to, Timestamp timestamp) {
        this->db_->disconnect(from, to, timestamp);
        auto message = Protocol::Message{Protocol::Payload{Protocol::ConnectDisconnect{from,
                                                                                       to,
                                                                                       Protocol::Change::Assert}},
                                         timestamp};
        this->network_->send(message);
    }
}

