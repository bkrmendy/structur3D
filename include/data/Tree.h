//
//  Tree.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 07..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef Tree_h
#define Tree_h

#include <memory>
#include <vector>

#include "Node.h"

namespace S3D {

struct Tree {
    const std::shared_ptr<Node> node;
    const std::vector<Tree> children;

    Tree(const std::shared_ptr<Node> node, const std::vector<Tree> children)
        : node{node}, children{children} { }
};

}


#endif /* Tree_h */
