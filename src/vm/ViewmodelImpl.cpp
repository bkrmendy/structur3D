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
#include <interactor/ModularInteractor.h>


#include "vm/ViewmodelImpl.h"
#include "document/DocumentImpl.h"

namespace S3D {
    ViewModelImpl::ViewModelImpl(std::unique_ptr<Database> db, std::unique_ptr<ClientEndpoint> network)
        : db_{std::move(db)}
        , network_{std::move(network)}
        , cancellables_{}
        , documents_{}
        , message_{}
        , currentDocument_{nullptr} {
        this->databaseInteractor_ = this->makeDBInteractor();
        this->documentInteractor_ = this->makeDocumentInteractor(this->databaseInteractor_);
        this->networkInteractor_ = this->makeNetworkInteractor(this->databaseInteractor_);
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
                                         this->documentInteractor_,
                                         std::make_unique<Graph>(edges, nodes),
                                         std::make_unique<MeshFactory>());
        }

    const std::unique_ptr<Document>& ViewModelImpl::document() const {
        return this->currentDocument_;
    }

    const std::string& ViewModelImpl::message() const {
        return this->message_;
    }

    std::shared_ptr<Interactor> ViewModelImpl::makeDBInteractor() const {
        return std::make_shared<ModularInteractor>(
                [&db = this->db_](const std::shared_ptr<Node>& node, const ID& document, Timestamp now) {
                    if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
                        db->create(node->id(), NodeType::Sphere, document, now);
                        db->upsert(sphere->uid, sphere->radius, now);
                        db->upsert(sphere->uid, sphere->coord, now);
                    } else if (auto setOp = std::dynamic_pointer_cast<SetOp>(node)) {
                        db->create(node->id(), NodeType::SetOperation, document, now);
                        db->upsert(setOp->uid, setOp->type, now);
                    }
                },
                [&db = this->db_](const ID &from, const ID &to, Timestamp now) {
                    db->connect(from, to, now);
                },
                [&db = this->db_](const ID &entity, const Attribute &attribute, Timestamp now) {
                    std::visit(overloaded {
                            [&db, &now, &entity](const Coord& coord) { db->upsert(entity, coord, now); },
                            [&db, &now, &entity](const Radius& radius) { db->upsert(entity, radius, now); },
                            [&db, &now, &entity](const SetOperationType& setOpType) { db->upsert(entity, setOpType, now); },
                            [&db, &now, &entity](const Name& name) { db->upsert(entity, name, now); },
                    }, attribute);
                },
                [this](const std::shared_ptr<Node>& node, const ID &document, Timestamp now) {
                    this->db_->remove(node->id(), document, now);
                    if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
                        this->db_->retract(sphere->uid, sphere->radius, now);
                        this->db_->retract(sphere->uid, sphere->coord, now);
                    } else if (auto setOp = std::dynamic_pointer_cast<SetOp>(node)) {
                        this->db_->retract(setOp->uid, setOp->type, now);
                    }
                },
                [this](const ID &from, const ID &to, Timestamp now) {
                    this->db_->disconnect(from, to, now);
                }
            );
    }

    std::shared_ptr<Interactor> ViewModelImpl::makeNetworkInteractor(std::shared_ptr<Interactor> dbInteractor) {
        return std::make_shared<ModularInteractor>(
                [&db = this->databaseInteractor_, &doc = this->document()](const std::shared_ptr<Node>& node, const ID& document, Timestamp timestamp) {
                    db->create(node, document, timestamp);
                    if (doc != nullptr
                        && doc->id() == document) {
                        doc->graph()->create(node);
                    }
                },
                [&db = this->databaseInteractor_, &doc = this->document()](const ID &from, const ID &to, Timestamp now) {
                    db->connect(from, to, now);
                    doc->graph()->connect(from, to);
                },
                [&db = this->databaseInteractor_, &doc = this->document()](const ID &entity, const Attribute &attribute, Timestamp now) {
                    db->upsert(entity, attribute, now);
                    std::visit(overloaded {
                            [&doc, &now, &entity](const Coord& coord) {
                                doc->graph()->access(entity, [&coord](auto node) {
                                    std::dynamic_pointer_cast<Sphere>(node)->coord = coord;
                                });
                            },
                            [&doc, &now, &entity](const Radius& radius) {
                                doc->graph()->access(entity, [&radius](auto node) {
                                    std::dynamic_pointer_cast<Sphere>(node)->radius = radius;
                                });
                            },
                            [&doc, &now, &entity](const SetOperationType& type) {
                                doc->graph()->access(entity, [type](auto node) {
                                    std::dynamic_pointer_cast<SetOp>(node)->type = type;
                                });
                            },
                            [](const Name& name) { /* TODO: implement */ }
                    }, attribute);
                },
                [&db = this->databaseInteractor_, &doc = this->document()](const std::shared_ptr<Node>& node, const ID &document, Timestamp timestamp) {
                    db->remove(node, document, timestamp);
                    doc->remove(node);
                },
                [&db = this->databaseInteractor_, &doc = this->document()](const ID &from, const ID &to, Timestamp timestamp) {
                    db->disconnect(from, to, timestamp);
                    doc->graph()->disconnect(from, to);
                }
        );
    }

    std::shared_ptr<Interactor> ViewModelImpl::makeDocumentInteractor(std::shared_ptr<Interactor> dbInteractor) {
        return std::make_shared<ModularInteractor>(
                [&db = this->databaseInteractor_, &net = this->network_](const std::shared_ptr<Node>& node, const ID& document, Timestamp now) {
                    db->create(node, document, now);
                    if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
                        auto message = Protocol::Message{Protocol::Payload{Protocol::CreateDelete{document,
                                                                                                  Protocol::Node{*sphere},
                                                                                                  Protocol::Change::Assert
                        }}, now};
                        net->send(message);

                    } else if (auto setOp = std::dynamic_pointer_cast<SetOp>(node)) {
                        auto message = Protocol::Message{Protocol::Payload{Protocol::CreateDelete{document,
                                                                                                  Protocol::Node{*setOp},
                                                                                                  Protocol::Change::Assert
                        }}, now};
                        net->send(message);
                    }
                },
                [&db = this->databaseInteractor_, &net = this->network_](const ID &from, const ID &to, Timestamp now) {
                    db->connect(from, to, now);
                    auto message = Protocol::Message{Protocol::Payload{Protocol::ConnectDisconnect{from,
                                                                                                   to,
                                                                                                   Protocol::Change::Assert}}, now};
                    net->send(message);
                    },
                [&db = this->databaseInteractor_, &net = this->network_](const ID &entity, const Attribute &attribute, Timestamp now) {
                    db->upsert(entity, attribute, now);
                    std::visit(overloaded {
                            [&now, &net, &entity](const Coord& coord) {
                                auto message = Protocol::Message{Protocol::Payload{Protocol::Update{entity,
                                                                                                    Protocol::Attribute{coord}}},
                                                                 now};
                                net->send(message);
                            },
                            [&now, &net, &entity](const Radius& radius) {
                                auto message = Protocol::Message{Protocol::Payload{Protocol::Update{entity,
                                                                                                    Protocol::Attribute{radius}}},
                                                                 now};
                                net->send(message);
                            },
                            [&now, &net, &entity](const SetOperationType& type) {
                                auto message = Protocol::Message{Protocol::Payload{Protocol::Update{entity,
                                                                                                    Protocol::Attribute{type}}},
                                                                 now};
                                net->send(message);
                            },
                            [&now, &net, &entity](const Name& name) { /* TODO: implement */}
                    }, attribute);
                },
                [&db = this->databaseInteractor_, &net = this->network_](const std::shared_ptr<Node>& node, const ID &document, Timestamp now) {
                    db->remove(node, document, now);
                    if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
                        auto message = Protocol::Message{Protocol::Payload{Protocol::CreateDelete{document,
                                                                                                  Protocol::Node{*sphere},
                                                                                                  Protocol::Change::Retract
                        }}, now};
                        net->send(message);

                    } else if (auto setOp = std::dynamic_pointer_cast<SetOp>(node)) {
                        auto message = Protocol::Message{Protocol::Payload{Protocol::CreateDelete{document,
                                                                                                  Protocol::Node{*setOp},
                                                                                                  Protocol::Change::Retract
                        }}, now};
                        net->send(message);
                    }
                },
                [&db = this->databaseInteractor_, &net = this->network_](const ID &from, const ID &to, Timestamp timestamp) {
                    db->disconnect(from, to, timestamp);
                    auto message = Protocol::Message{Protocol::Payload{Protocol::ConnectDisconnect{from,
                                                                                                   to,
                                                                                                   Protocol::Change::Assert}},
                                                     timestamp};
                    net->send(message);
                }
        );
    }
}

