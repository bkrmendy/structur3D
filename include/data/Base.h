//
//  Base.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 04..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef Base_h
#define Base_h

#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/uuid/uuid_generators.hpp"

namespace S3D {
using ID = boost::uuids::uuid;
using IDFactory = boost::uuids::random_generator;

}


#endif /* Base_h */
