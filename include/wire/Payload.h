//
// Created by Berci on 2020. 11. 19..
//

#ifndef STRUCTUR3D_BASE_PAYLOAD_H
#define STRUCTUR3D_BASE_PAYLOAD_H

#include <variant>

#include <bitsery/bitsery.h>
#include <bitsery/ext/std_variant.h>
#include <bitsery/ext/std_tuple.h>
#include <bitsery/ext/compact_value.h>

#include <bitsery/brief_syntax.h>
#include <bitsery/brief_syntax/string.h>

#include <data/Coord.h>
#include <data/Base.h>
#include <data/SetOperationType.h>
#include <db/NodeType.h>

namespace S3D {
    using Payload = std::variant<Coord, RADIUS, SetOperationType, std::string, std::tuple<NodeType, ID>>;

    template <typename S>
    void serialize(S& s, Payload& payload) {
        s.ext(payload, bitsery::ext::StdVariant {
           [](S& s, Coord& coord) {
               s.value4b(coord.x);
               s.value4b(coord.y);
               s.value4b(coord.z);
           },
           [](S& s, RADIUS& radius) { s.value4b(radius); },
           [](S& s, SetOperationType& setOp) { s.value4b(setOp); },
           [](S& s, std::string& str) {
               s(str);
           },
           [](S& s, std::tuple<NodeType, ID>& info) {
               s.ext(info, bitsery::ext::StdTuple{
                  [](S& s, NodeType& type) { s.value4b(type); },
                  [](S& s, ID& id) { s.container(id.data, [](S& s, uint8_t byte) { s.value1b(byte); }); }
               });
           }
        });
    }
}

#endif //STRUCTUR3D_BASE_PAYLOAD_H
