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

    Tree DocumentImpl::subTreeOf(const std::shared_ptr<Node> node) const {
    std::vector<Tree> subtrees{};
    for (const auto& edge : this->graph_->edges) {
        if (edge->from->id() == node->id()) {
            subtrees.push_back(this->subTreeOf(edge->to));
        }
    }
    return Tree{node, subtrees};
}

    std::vector<Tree> DocumentImpl::trees() const {
    std::vector<Tree> res{};
    for (auto& root : this->graph_->roots()) {
        res.push_back(this->subTreeOf(root));
    }
    return res;
}

    const std::unique_ptr<Graph>& DocumentImpl::graph() const {
        return this->graph_;
    }

    void DocumentImpl::create(const std::shared_ptr<Node> node) {
        if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
            this->db->create(node->id(), NodeType::Sphere, id_);
            this->db->upsert(sphere->uid, sphere->radius);
            this->db->upsert(sphere->uid, sphere->coord);
        } else if (auto setop = std::dynamic_pointer_cast<SetOp>(node)) {
            this->db->create(node->id(), NodeType::SetOperation, id_);
            this->db->upsert(setop->uid, setop->type);
        }

        this->graph_->nodes.push_back(node);
        this->regenMesh();
    }

    void DocumentImpl::remove(const std::shared_ptr<Node> node) {
        this->db->remove(node->id(), id_);
        if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
            this->db->retract(sphere->uid, sphere->radius);
            this->db->retract(sphere->uid, sphere->coord);
        } else if (auto setop = std::dynamic_pointer_cast<SetOp>(node)) {
            this->db->retract(setop->uid, setop->type);
        }

        for (const auto& edge : this->graph_->edges) {
            if (edge->to->id() == node->id()) {
                this->db->disconnect(edge->from->id(), edge->to->id());
            }
        }

        this->graph_->nodes.erase(std::remove(this->graph_->nodes.begin(), this->graph_->nodes.end(), node));
        this->graph_->edges.erase(
            std::remove_if(
                this->graph_->edges.begin(),
                this->graph_->edges.end(),
                [node](const std::shared_ptr<Edge> edge){ return edge->from == node || edge->to == node; }
            )
        );

        this->regenMesh();
    }

    void DocumentImpl::create(const std::shared_ptr<Edge> edge) {
        this->graph_->edges.push_back(edge);
        this->db->connect(edge->from->id(), edge->to->id());
        this->regenMesh();
    }

    void DocumentImpl::remove(const std::shared_ptr<Edge> edge) {
        this->graph_->edges.erase(std::remove(this->graph_->edges.begin(), this->graph_->edges.end(), edge));
        this->db->disconnect(edge->from->id(), edge->to->id());
        this->regenMesh();
    }

    void DocumentImpl::regenMesh() {
        auto gen = std::async(std::launch::async, [this, forest = this->trees()](){
            for (auto& tree : forest) {
                this->meshes_.push_back(Mesh::fromTree(tree));
            }
        });

        this->cancellables.push_back(std::move(gen));
    }

    void DocumentImpl::update(const std::shared_ptr<Node> node) {
        if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
            this->db->upsert(sphere->uid, sphere->radius);
            this->db->upsert(sphere->uid, sphere->coord);
        } else if (auto setop = std::dynamic_pointer_cast<SetOp>(node)) {
            this->db->upsert(setop->uid, setop->type);
        }
        this->regenMesh();
    }

    const std::vector<std::unique_ptr<Mesh>>& DocumentImpl::meshes() const {
        return this->meshes_;
    }

    ID DocumentImpl::id() const {
        return this->id_;
    }

}
