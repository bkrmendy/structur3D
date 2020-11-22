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

namespace S3D {
    void DocumentImpl::create(const std::shared_ptr<Node> node) {
        this->interactor_->create(node, this->id_);
        this->graph_->nodes.push_back(node);
        this->regenMesh();
    }

    void DocumentImpl::remove(const std::shared_ptr<Node> node) {
        this->interactor_->remove(node, this->id_);

        for (const auto& edge : this->graph_->edges) {
            if (edge->to->id() == node->id()) {
                this->interactor_->disconnect(edge->from->id(), edge->to->id());
            }
        }

        this->graph_->nodes.erase(
                std::remove(this->graph_->nodes.begin(), this->graph_->nodes.end(), node),
                this->graph_->nodes.end());

        this->graph_->edges.erase(
                std::remove_if(
                    this->graph_->edges.begin(),
                    this->graph_->edges.end(),
                    [&node](const auto& edge) { return edge->from->id() == node->id() || edge->to->id() == node->id();}),
                    this->graph_->edges.end());

        this->regenMesh();
    }

    void DocumentImpl::create(const std::shared_ptr<Edge> edge) {
        this->interactor_->connect(edge->from->id(), edge->to->id());
        this->graph_->edges.push_back(edge);
        this->regenMesh();
    }

    void DocumentImpl::remove(const std::shared_ptr<Edge> edge) {
        this->graph_->edges.erase(
                std::remove(this->graph_->edges.begin(), this->graph_->edges.end(), edge),
                this->graph_->edges.end());
        auto now = TimestampFactory().timestamp();
        this->interactor_->disconnect(edge->from->id(), edge->to->id());
        this->regenMesh();
    }

    void DocumentImpl::update(const std::shared_ptr<Node> node) {
        // TODO
//        auto now = TimestampFactory().timestamp();
//        if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
//            this->db->upsert(sphere->uid, sphere->radius, now);
//            this->db->upsert(sphere->uid, sphere->coord, now);
//        } else if (auto setop = std::dynamic_pointer_cast<SetOp>(node)) {
//            this->db->upsert(setop->uid, setop->type, now);
//        }
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
