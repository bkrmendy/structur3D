//
// Created by Berci on 2020. 11. 19..
//

#ifndef STRUCTUR3D_BASE_PAYLOAD_H
#define STRUCTUR3D_BASE_PAYLOAD_H

#include <variant>

#include <bitsery/bitsery.h>
#include <bitsery/ext/std_variant.h>

#include <data/Coord.h>
#include <data/Base.h>
#include <data/SetOperationType.h>
#include <db/NodeType.h>

namespace S3D {
    using Payload = std::variant<Coord, RADIUS, SetOperationType, std::string, std::tuple<NodeType, ID>>;

    template <typename S>
    void serialize(S& s, Payload& payload) {
        
    }
}

#endif //STRUCTUR3D_BASE_PAYLOAD_H
