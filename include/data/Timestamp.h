//
//  Timestamp.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 11..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef Timestamp_h
#define Timestamp_h

#include <chrono>

namespace S3D {
using Timestamp = std::uint64_t;

struct TimestampFactory {
    virtual Timestamp timestamp() const;
};
}

#endif /* Timestamp_h */
