//
//  NodeType.cpp
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 11..
//  Copyright © 2020. Berci. All rights reserved.
//

#include "db/NodeType.h"

namespace S3D {

int to_integral(const NodeType& type) {
    switch (type) {
        case NodeType::Sphere: return 0;
        case NodeType::SetOperation: return 1;
    }
}

NodeType from_integral(int integral) {
    if (integral == 0) { return NodeType::Sphere; }
    if (integral == 1) { return NodeType::SetOperation; }
    throw 0;
}

}
