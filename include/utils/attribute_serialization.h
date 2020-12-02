//
// Created by Berci on 2020. 12. 02..
//

#ifndef STRUCTUR3D_BASE_ATTRIBUTE_SERIALIZATION_H
#define STRUCTUR3D_BASE_ATTRIBUTE_SERIALIZATION_H

#include <bitsery/bitsery.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/array.h>

#include <data/Name.h>
#include "data/Base.h"
#include "data/SetOperationType.h"
#include "data/Coord.h"
#include "data/Radius.h"

namespace S3D {
    template <typename S>
    void serialize(S& s, ID& uid) {
        s.container<16>(uid.data);;
    }
    template <typename S>
    void serialize(S& s, boost::uuids::uuid uid) {
        s.container<16>(uid.data);;
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
    void serialize(S& s, Name& name) {
        auto str = name.get();
        s.template text<sizeof(std::string::value_type)>(str, 1000);
    }
}

#endif //STRUCTUR3D_BASE_ATTRIBUTE_SERIALIZATION_H
