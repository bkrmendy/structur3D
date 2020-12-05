//
// Created by Berci on 2020. 12. 05..
//

#include "db/attribute_priority.h"

namespace S3D {
    Radius preferred_radius(const Radius& left, const Radius& right) {
        if (left.magnitude() < right.magnitude()) {
            return left;
        }
        return right;
    }

    Coord preferred_coord(const Coord& left, const Coord& right) {
        if (distance_from_origin(left) < distance_from_origin(right)) {
            return left;
        }
        return right;
    }

    Name preferred_name(const Name& left, const Name& right) {
        if (left.get() < right.get()) {
            return left;
        }
        return right;
    }

    SetOperationType preferred_setoperation_type(const SetOperationType& left, const SetOperationType& right) {
        if (left == SetOperationType::Union) {
            return left;
        }
        if (right == SetOperationType::Union) {
            return right;
        }
        return left;
    }
}

