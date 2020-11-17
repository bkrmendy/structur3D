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

Timestamp make_timestamp() {
    auto stamp = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(stamp);
    return millis.count();
}
}

#endif /* Timestamp_h */
