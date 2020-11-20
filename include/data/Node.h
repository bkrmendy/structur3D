//
//  Node.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 11..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef Node_h
#define Node_h

#include "Base.h"

namespace S3D {
struct Node {
    virtual ID id() const = 0;
    virtual ~Node() = default;
};
}

#endif /* Node_h */
