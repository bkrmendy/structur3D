//
//  DocumentImpl.cpp
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 14..
//  Copyright © 2020. Berci. All rights reserved.
//

#include <future>

#include "document/DocumentImpl.h"
#include "data/Sphere.h"
#include "data/SetOp.h"
#include "data/Attribute.h"

namespace S3D {
    void DocumentImpl::create(const std::shared_ptr<Node> node) {
        auto now = TimestampFactory().timestamp();
        this->interactor_->create(node, this->id_, now);
        this->regenMesh();
    }

    void DocumentImpl::remove(const std::shared_ptr<Node> node) {
        auto now = TimestampFactory().timestamp();
        this->interactor_->remove(node, this->id_, now);
        this->regenMesh();
    }

    void DocumentImpl::create(const std::shared_ptr<Edge> edge) {
        auto now = TimestampFactory().timestamp();
        this->interactor_->connect(edge->from->id(), edge->to->id(), now);
        this->regenMesh();
    }

    void DocumentImpl::remove(const std::shared_ptr<Edge> edge) {
        auto now = TimestampFactory().timestamp();
        this->interactor_->disconnect(edge->from->id(), edge->to->id(), now);
        this->regenMesh();
    }

    void DocumentImpl::update(const std::shared_ptr<Node> node) {
        auto now = TimestampFactory().timestamp();
        if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
            this->interactor_->upsert(sphere->uid, Attribute{sphere->radius}, now);
            this->interactor_->upsert(sphere->uid, Attribute{sphere->coord}, now);
        } else if (auto setOp = std::dynamic_pointer_cast<SetOp>(node)) {
            this->interactor_->upsert(setOp->uid, Attribute{setOp->type}, now);
        }
        this->regenMesh();
    }

    const std::unique_ptr<Graph>& DocumentImpl::graph() const {
        return this->graph_;
    }

    void DocumentImpl::regenMesh() {
        auto gen = std::async(std::launch::async, [this, forest = this->graph()->trees()](){
            this->mesh_ = factory_->fromTree(forest);
        });

        this->cancellables.push_back(std::move(gen));
    }

    const std::unique_ptr<Mesh>& DocumentImpl::mesh() const {
        return this->mesh_;
    }

    const ID DocumentImpl::id() const {
        return this->id_;
    }

}
