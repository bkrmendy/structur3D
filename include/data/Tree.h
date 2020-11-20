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
#include <utility>
#include <vector>

#include "Node.h"

namespace S3D {

struct Tree {
    const std::shared_ptr<Node> node;
    const std::vector<Tree> children;

    Tree(std::shared_ptr<Node>  node, std::vector<Tree>  children)
        : node{std::move(node)}, children{std::move(children)} { }
};

}


#endif /* Tree_h */
