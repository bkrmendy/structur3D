//
// Created by Berci on 2020. 12. 05..
//

#include "db/attribute_priority.h"

namespace S3D {
    Radius preferred(const Radius& left, const Radius& right) {
        if (left.magnitude() < right.magnitude()) {
            return left;
        }
        return right;
    }

    Coord preferred(const Coord& left, const Coord& right) {
        if (distance_from_origin(left) < distance_from_origin(right)) {
            return left;
        }
        return right;
    }

    Name preferred(const Name& left, const Name& right) {
        if (left.get() < right.get()) {
            return left;
        }
        return right;
    }

    SetOperationType preferred(const SetOperationType& left, const SetOperationType& right) {
        if (left == SetOperationType::Union) {
            return left;
        }
        if (right == SetOperationType::Union) {
            return right;
        }
        return left;
    }
}

