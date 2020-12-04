//
// Created by Berci on 2020. 12. 02..
//

#ifndef STRUCTUR3D_BASE_ATTRIBUTE_SERIALIZATION_H
#define STRUCTUR3D_BASE_ATTRIBUTE_SERIALIZATION_H

#include <bitsery/bitsery.h>
#include <bitsery/traits/array.h>
#include <bitsery/traits/string.h>

#include "data/Name.h"
#include "data/Base.h"
#include "data/SetOperationType.h"
#include "data/Coord.h"
#include "data/Radius.h"

namespace S3D {
    template <typename S>
    void serialize(S& s, std::string& str) {
        s.template text<sizeof(std::string::value_type)>(str, 1000);
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
        s.value4b(radius.magnitude());
    }

    template <typename S>
    void serialize(S& s, Name& name) {
        s.template text<sizeof(std::string::value_type)>(name.value_, 1000);
    }
}

#endif //STRUCTUR3D_BASE_ATTRIBUTE_SERIALIZATION_H
