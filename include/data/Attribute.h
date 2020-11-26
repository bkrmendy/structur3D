//
// Created by Berci on 2020. 11. 22..
//

#ifndef STRUCTUR3D_BASE_ATTRIBUTE_H
#define STRUCTUR3D_BASE_ATTRIBUTE_H

#include <variant>
#include "Coord.h"
#include "Base.h"
#include "Radius.h"
#include "SetOperationType.h"
#include "Name.h"

namespace S3D {
    using Attribute = std::variant<Coord, Radius, SetOperationType, Name>;
}

#endif //STRUCTUR3D_BASE_ATTRIBUTE_H
