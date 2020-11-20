//
// Created by Berci on 2020. 11. 20..
//

#include "data/Timestamp.h"

namespace S3D {
    Timestamp TimestampFactory::timestamp() const {
        auto stamp = std::chrono::system_clock::now().time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(stamp);
        return millis.count();
    }
}