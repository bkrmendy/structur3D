//
//  Edge.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 07..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef Edge_h
#define Edge_h

#include <memory>

#include "Base.h"
#include "Node.h"

namespace S3D {
class Edge {
public:
    const ID uid;
    const std::shared_ptr<Node> from;
    const std::shared_ptr<Node> to;

    Edge(const ID uid, const std::shared_ptr<Node>& from, const std::shared_ptr<Node>& to)
        : uid{uid}, from{from}, to{to} {
            assert((from->id() != to->id()));
        }
    ~Edge() {}
};
}


#endif /* Edge_h */
