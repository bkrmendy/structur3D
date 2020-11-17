//
//  IDWithName.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 14..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef IDWithName_h
#define IDWithName_h

#include "./NodeType.h"
#include "../data/Base.h"

namespace S3D {

struct IDWithType {
    const ID uid;
    const NodeType type;

    IDWithType(const ID& uid, const NodeType& type) : uid{uid}, type{type} { }
};

}

#endif /* IDWithName_h */
