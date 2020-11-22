//
// Created by Berci on 2020. 11. 22..
//
#include <cmath>
#include "data/Coord.h"

namespace S3D {
    float distance_from_origin(const Coord& coord) {
        return sqrt(coord.x * coord.x
                    + coord.y * coord.y
                    + coord.z * coord.z);
    }
}
