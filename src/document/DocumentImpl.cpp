//
//  DocumentImpl.cpp
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 14..
//  Copyright © 2020. Berci. All rights reserved.
//

#include "document/DocumentImpl.h"

Tree DocumentImpl::subTreeOf(const std::shared_ptr<Node> node) const {
    std::vector<Tree> subtrees{};
    for (const auto& edge : this->graph_->edges) {
        if (edge->from->getID() == node->getID()) {
            subtrees.push_back(this->subTreeOf(edge->to));
        }
    }
    return Tree{node, subtrees};
}

const std::vector<Tree> DocumentImpl::trees() const {
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
    //    this->db->create(node->getID(), ...);

    if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
        this->db->upsert(sphere->uid, sphere->radius);
        this->db->upsert(sphere->uid, sphere->coord);
    } else if (auto setop = std::dynamic_pointer_cast<SetOp>(node)) {
        this->db->upsert(setop->uid, setop->type);
    }

    this->graph_->nodes.push_back(node);
    this->regenMesh();
}

void DocumentImpl::remove(const std::shared_ptr<Node> node) {
    //    this->db->remove(node->getID(), ...);

    if (auto sphere = std::dynamic_pointer_cast<Sphere>(node)) {
        this->db->retract(sphere->uid, sphere->radius);
        this->db->retract(sphere->uid, sphere->coord);
    } else if (auto setop = std::dynamic_pointer_cast<SetOp>(node)) {
        this->db->retract(setop->uid, setop->type);
    }

    for (const auto& edge : this->graph_->edges) {
        if (edge->to->getID() == node->getID()) {
            this->db->disconnect(edge->from->getID(), edge->to->getID());
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
    this->db->connect(edge->from->getID(), edge->to->getID());
    this->regenMesh();
}

void DocumentImpl::remove(const std::shared_ptr<Edge> edge) {
    this->graph_->edges.erase(std::remove(this->graph_->edges.begin(), this->graph_->edges.end(), edge));
    this->db->disconnect(edge->from->getID(), edge->to->getID());
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
