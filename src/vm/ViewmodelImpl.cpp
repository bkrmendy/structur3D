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

ViewModelImpl::ViewModelImpl(std::unique_ptr<Database> db, std::unique_ptr<Network> network)
    : db_{std::move(db)}
    , network_{std::move(network)}
    , cancellables_{}
    , documents_{}
    , message_{} {
        this->currentDocument_ = nullptr;
        this->documents_ = this->db_->documents();
}

void ViewModelImpl::createDocument(const std::string& name) {
    auto makeID = IDFactory();
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

    void ViewModelImpl::create(const std::shared_ptr<Node> node, const ID& document) {
        auto now = TimestampFactory().timestamp();
        if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
            this->db_->create(node->id(), NodeType::Sphere, document, now);
            this->db_->upsert(sphere->uid, sphere->radius, now);
            this->db_->upsert(sphere->uid, sphere->coord, now);
        } else if (auto setOp = std::dynamic_pointer_cast<SetOp>(node)) {
            this->db_->create(node->id(), NodeType::SetOperation, document, now);
            this->db_->upsert(setOp->uid, setOp->type, now);
        }
    }

    void ViewModelImpl::connect(const ID &from, const ID &to) {
        auto now = TimestampFactory().timestamp();
        this->db_->connect(from, to, now);
    }

    void ViewModelImpl::update(const ID& entity, const Attribute& attribute) {
        auto now = TimestampFactory().timestamp();
        std::visit(overloaded {
            [this, &now, &entity](const Coord& coord) { this->db_->upsert(entity, coord, now); },
            [this, &now, &entity](const Radius& radius) { this->db_->upsert(entity, radius, now); },
            [this, &now, &entity](const SetOperationType& type) { this->db_->upsert(entity, type, now); },
            [this, &now, &entity](const std::string& name) { this->db_->upsert(entity, name, now); }
        }, attribute);
    }

    void ViewModelImpl::remove(std::shared_ptr <Node> node, const ID &document) {
        auto now = TimestampFactory().timestamp();
        this->db_->remove(node->id(), document, now);
        if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
            this->db_->retract(sphere->uid, sphere->radius, now);
            this->db_->retract(sphere->uid, sphere->coord, now);
        } else if (auto setOp = std::dynamic_pointer_cast<SetOp>(node)) {
            this->db_->retract(setOp->uid, setOp->type, now);
        }
    }

    void ViewModelImpl::disconnect(const ID &from, const ID &to) {
        auto now = TimestampFactory().timestamp();
        this->db_->disconnect(from, to, now);
    }
}

