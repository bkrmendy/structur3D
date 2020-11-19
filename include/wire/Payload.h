//
// Created by Berci on 2020. 11. 19..
//

#ifndef STRUCTUR3D_BASE_PAYLOAD_H
#define STRUCTUR3D_BASE_PAYLOAD_H

#include <variant>

#include <bitsery/bitsery.h>
#include <bitsery/ext/std_variant.h>
#include <bitsery/ext/std_tuple.h>

#include <data/Coord.h>
#include <data/Base.h>
#include <data/SetOperationType.h>
#include <db/NodeType.h>

namespace S3D {
    using Payload = std::variant<Coord, RADIUS, SetOperationType, std::string, std::tuple<NodeType, ID>>;

    template <typename S>
    void serialize(S& s, Payload& payload) {
        s.ext(payload, bitsery::ext::StdVariant {
           [](S&s, Coord& coord) {
               s.value4b(coord.x);
               s.value4b(coord.y);
               s.value4b(coord.z);
           },
           [](S&s, RADIUS& radius) { s.writeBytes<sizeof(RADIUS)>(radius); },
           [](S&s, SetOperationType& setOp) { s.writeBytes<sizeof(SetOperationType)>(setOp); },
           [](S&s, std::string& str) {
               s.container(str, str.size(), [](S&s, char c) { s.value1b(c); });
           },
           [](S&s, std::tuple<NodeType, ID>& info) {
               s.ext(info, bitsery::ext::StdTuple{
                  [](S&s, NodeType& type) { s.writeBytes<sizeof(NodeType)>(type); }
                  [](S&s, ID& uid) { s.writeBytes<sizeof(ID)>(uid); }
               });
           }
        });
    }
}

#endif //STRUCTUR3D_BASE_PAYLOAD_H
