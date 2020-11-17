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
class Node {
public:
    virtual ID getID() const  = 0;
    virtual ~Node(){};
};
}

#endif /* Node_h */
