//
// Created by Berci on 2020. 11. 22..
//
#include <cmath>
#include "utils/approximately_equal.h"
#include "data/Coord.h"

namespace S3D {
    float distance_from_origin(const Coord& coord) {
        return sqrt(coord.x * coord.x
                    + coord.y * coord.y
                    + coord.z * coord.z);
    }

    bool operator==(const Coord& a, const Coord& b) {
        const float tolerance = 0.0001;
        return (
                approximately_equal(a.x, b.x, tolerance)
                && approximately_equal(a.y, b.y, tolerance)
                && approximately_equal(a.z, b.z, tolerance)
                );
    }

    std::ostream& operator<<(std::ostream& os, const Coord& coord) {
        return os << "[Coord x: " << coord.x << " y: " << coord.y << " z: " << coord.z << "]";
    }

}
