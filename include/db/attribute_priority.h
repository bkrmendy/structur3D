//
// Created by Berci on 2020. 12. 05..
//

#ifndef STRUCTUR3D_BASE_ATTRIBUTE_PRIORITY_H
#define STRUCTUR3D_BASE_ATTRIBUTE_PRIORITY_H

#include <data/Radius.h>
#include <data/Coord.h>
#include <data/Name.h>
#include <data/SetOperationType.h>

namespace S3D {
    Radius preferred(const Radius& left, const Radius& right);
    Coord preferred(const Coord& left, const Coord& right);
    Name preferred(const Name& left, const Name& right);
    SetOperationType preferred(const SetOperationType& left, const SetOperationType& right);
}

#endif //STRUCTUR3D_BASE_ATTRIBUTE_PRIORITY_H
