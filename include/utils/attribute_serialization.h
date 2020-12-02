//
// Created by Berci on 2020. 12. 02..
//

#ifndef STRUCTUR3D_BASE_ATTRIBUTE_SERIALIZATION_H
#define STRUCTUR3D_BASE_ATTRIBUTE_SERIALIZATION_H

#include "data/Base.h"
#include "data/SetOperationType.h"
#include "data/Coord.h"
#include "data/Radius.h"

namespace S3D {
    template <typename S>
    void serialize(S& s, ID& uid) {
        s.container(uid.data, [](S& s, uint8_t byte) { s.value1b(byte); });;
    }

    template <typename S>
    void serialize(S& s, SetOperationType& setOp) {
        s.value4b(setOp);
    }

    template <typename S>
    void serialize(S& s, Coord& coord) {
        s.value4b(coord.x);
        s.value4b(coord.y);
        s.value4b(coord.z);
    }

    template <typename S>
    void serialize(S& s, Radius& radius) {
        s.value4b(radius);
    }

    template <typename S>
    void serialize(S& s, std::string str) {
        s(str);
    }
}

#endif //STRUCTUR3D_BASE_ATTRIBUTE_SERIALIZATION_H
