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

struct Graph {
    const std::vector<std::shared_ptr<Node>> roots() const;
    Tree subTreeOf(const std::shared_ptr<Node>& node) const;

    std::vector<std::shared_ptr<Edge>> edges;
    std::vector<std::shared_ptr<Node>> nodes;

    Graph(std::vector<std::shared_ptr<Edge>> edges, std::vector<std::shared_ptr<Node>> nodes)
        : edges{std::move(edges)}, nodes{std::move(nodes)} { }

    const std::vector<Tree> trees() const;

};

}

#endif /* Graph_h */
