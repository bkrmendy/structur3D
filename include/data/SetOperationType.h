//
//  SetOperationType.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 11..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef SetOperationType_h
#define SetOperationType_h]

#include <optional>

namespace S3D {
enum class SetOperationType {
    Union,
    Subtraction,
    Intersection
};

size_t from_operationType(const SetOperationType& type);

std::optional<SetOperationType> to_operationType(size_t typeId);

}

#endif /* SetOperationType_h */
