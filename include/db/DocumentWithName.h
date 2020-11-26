//
//  DocumentWithName.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 14..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef DocumentWithName_h
#define DocumentWithName_h

#include <string>
#include <utility>

#include "../data/Base.h"

namespace S3D {

struct DocumentWithName {
    const ID uid;
    const Name name;

    DocumentWithName(const ID& uid, Name name) : uid{uid}, name{std::move(name)} { }
};

}


#endif /* DocumentWithName_h */
