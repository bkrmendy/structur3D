//
//  Graph.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 07..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef Graph_h
#define Graph_h

#include <memory>
#include <vector>

#include "Base.h"
#include "Edge.h"
#include "Tree.h"

namespace S3D {

class Graph {
    std::vector<std::shared_ptr<Edge>> edges_;
    std::vector<std::shared_ptr<Node>> nodes_;
public:
    Graph(std::vector<std::shared_ptr<Edge>> edges, std::vector<std::shared_ptr<Node>> nodes)
        : edges_{std::move(edges)}, nodes_{std::move(nodes)} { }

    const std::vector<std::shared_ptr<Node>> roots() const;
    Tree subTreeOf(const std::shared_ptr<Node>& node) const;
    const std::vector<Tree> trees() const;

    const std::vector<std::shared_ptr<Edge>>& edges() const;
    const std::vector<std::shared_ptr<Node>>& nodes() const;

    void create(std::shared_ptr<Node> node);
    void remove(std::shared_ptr<Node> node);

    void access(const ID& uid, std::function<void(std::shared_ptr<Node>)>&& action);

    void connect(const ID& from, const ID& to);
    void disconnect(const ID& from, const ID& to);
};

}

#endif /* Graph_h */
