//
// Created by Berci on 2020. 11. 22..
//

#include <cassert>
#include "data/Radius.h"

namespace S3D {
    Radius::Radius(float magnitude) {
        assert(magnitude >= 0);
        this->magnitude_ = magnitude;
    }

    float Radius::magnitude() const { return this->magnitude_; }
}