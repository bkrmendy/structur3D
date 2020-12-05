//
// Created by Berci on 2020. 11. 17..
//

#include "data/SetOperationType.h"

namespace S3D {
    size_t from_operationType(const SetOperationType &type) {
        switch (type) {
            case SetOperationType::Union:
                return 0;
            case SetOperationType::Intersection:
                return 1;
        }
    }

    std::optional<SetOperationType> to_operationType(size_t typeId) {
        if (typeId == 0) { return SetOperationType::Union; }
        if (typeId == 1) { return SetOperationType::Intersection; }
        return std::nullopt;
    }
}