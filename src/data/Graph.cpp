//
//  Graph.cpp
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 13..
//  Copyright © 2020. Berci. All rights reserved.
//

#include <map>
#include <vector>

#include "data/Graph.h"

namespace S3D {
    const std::vector<std::shared_ptr<Node>> Graph::roots() const {
        std::map<std::shared_ptr<Node>, size_t> edges_in;
        for (const auto& node : this->nodes_) {
            edges_in.insert(std::make_pair(node, 0));
        }

        for (const auto& edge : this->edges_) {
            const auto& to = edge->to;
            edges_in.at(to) += 1;
        }

        std::vector<std::shared_ptr<Node>> res;
        for (auto& entry : edges_in) {
            if (entry.second == 0) {
                res.push_back(entry.first);
            }
        }

        return res;
    }

    Tree Graph::subTreeOf(const std::shared_ptr<Node>& node) const {
        std::vector<Tree> subtrees{};
        for (const auto& edge : this->edges_) {
            if (edge->from->id() == node->id()) {
                subtrees.push_back(this->subTreeOf(edge->to));
            }
        }
        return Tree{node, subtrees};
    }

    const std::vector<Tree> Graph::trees() const {
        std::vector<Tree> res{};
        for (const auto& root : this->roots()) {
            res.push_back(this->subTreeOf(root));
        }
        return res;
    }

    const std::vector<std::shared_ptr<Edge>>& Graph::edges() const {
        return edges_;
    }

    const std::vector<std::shared_ptr<Node>>& Graph::nodes() const {
        return nodes_;
    }

    void Graph::create(std::shared_ptr<Node> node) {
        this->nodes_.push_back(std::move(node));
    }

    void Graph::remove(std::shared_ptr<Node> node) {
        this->nodes_.erase(
                std::remove(this->nodes_.begin(), this->nodes_.end(), node),
                this->nodes_.end());

        this->edges_.erase(
                std::remove_if(
                        this->edges_.begin(),
                        this->edges_.end(),
                        [&node](const auto& edge) { return edge->from->id() == node->id() || edge->to->id() == node->id();}),
                this->edges_.end());
    }

    void Graph::create(std::shared_ptr<Edge> edge) {
        this->edges_.push_back(std::move(edge));
    }

    void Graph::remove(std::shared_ptr<Edge> edge) {
        this->edges_.erase(
                std::remove(this->edges_.begin(), this->edges_.end(), edge),
                this->edges_.end());
    }

    void Graph::access(const ID &uid, std::function<void(std::shared_ptr<Node>)>&& action) {
        for (size_t i = 0; i < nodes_.size(); i++) {
            if (nodes_.at(i)->id() == uid) {
                action(nodes_.at(0));
            }
        }
    }
}
