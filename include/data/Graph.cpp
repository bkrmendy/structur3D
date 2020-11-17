//
//  Graph.cpp
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 13..
//  Copyright © 2020. Berci. All rights reserved.
//

#include <map>
#include <vector>

#include "./Graph.h"

namespace S3D {
const std::vector<std::shared_ptr<Node>> Graph::roots() const {
    std::map<std::shared_ptr<Node>, size_t> edges_in;
    for (const auto& node : this->nodes) {
        edges_in.insert(std::make_pair(node, 0));
    }

    for (const auto& edge : this->edges) {
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
}
