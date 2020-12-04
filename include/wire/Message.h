//
// Created by Berci on 2020. 11. 19..
//

#ifndef STRUCTUR3D_BASE_MESSAGE_H
#define STRUCTUR3D_BASE_MESSAGE_H

#include <utility>

#include <bitsery/bitsery.h>
#include <bitsery/traits/array.h>
#include <bitsery/ext/compact_value.h>
#include <bitsery/ext/std_variant.h>
#include <bitsery/ext/std_tuple.h>

#include <bitsery/brief_syntax.h>
#include <bitsery/brief_syntax/string.h>

#include <data/Base.h>
#include <data/Timestamp.h>
#include <data/Sphere.h>
#include <data/SetOp.h>

#include <db/NodeType.h>
#include <data/Name.h>

#include <utils/attribute_serialization.h>

namespace S3D {
    namespace Protocol {
        enum class Change {
            Assert,
            Retract
        };

        using Node = std::variant<Sphere, SetOp>;
        template <typename S>
        void serialize(S& s, Node& node) {
            s.ext(node, bitsery::ext::StdVariant{
                    [](S &s, Sphere &sphere) {
                        serialize(s, sphere.uid);
                        serialize(s, sphere.coord);
                        serialize(s, sphere.radius);
                    },
                    [](S &s, SetOp &setOp) {
                        serialize(s, setOp.uid);
                        serialize(s, setOp.type);
                    }
            });
        }

        struct ConnectDisconnect {
            const ID from;
            const ID to;
            const Change deleted;

            ConnectDisconnect() = delete;

            ConnectDisconnect(const ID &from, const ID &to, const Change deleted)
                : from(from)
                , to(to)
                , deleted(deleted)
                {}
        };
        template <typename S>
        void serialize(S& s, ConnectDisconnect& edge) {
            serialize(s, edge.from);
            serialize(s, edge.to);
            s.value4b(edge.deleted);
        }

        using Attribute = std::variant<Coord, Radius, SetOperationType, Name>;

        struct CreateDelete {
            ID document;
            Node node;
            Change create;

            CreateDelete(const ID &document, Node node, Change create)
                : document(document)
                , node(std::move(node))
                , create(create)
                {}
        };
        template <typename S>
        void serialize(S& s, CreateDelete& create) {
            serialize(s, create.document);
            serialize(s, create.node);
            s.value4b(create.create);
        }

        struct Update {
            const ID uid;
            const Attribute attribute;

            Update() = delete;

            Update(const ID &uid, Attribute attribute) : uid(uid), attribute(std::move(attribute)) {}
        };
        template <typename S>
        void serialize(S& s, Update& update) {
            serialize(s, update.uid);
            s.ext(update.attribute, bitsery::ext::StdVariant {
                    [](S& s, Coord& coord) { serialize(s, coord); },
                    [](S& s, Radius& radius) { serialize(s, radius); },
                    [](S& s, SetOperationType& setOp) { serialize(s, setOp); },
                    [](S& s, Name& name) { serialize(s, name); },
                    [](S& s, std::tuple<NodeType, ID>& info) {
                        s.ext(info, bitsery::ext::StdTuple{
                                [](S& s, NodeType& type) { s.value4b(type); },
                                [](S& s, ID& uid) { serialize(s, uid); }
                        });
                    }
            });
        }

        struct Confirm {
            int64_t placeholder = 0;
        };
        using Payload = std::variant<ConnectDisconnect, Update, CreateDelete, Confirm>;
        template<typename S>
        void serialize(S& s, Payload& payload) {
            s.ext(payload, bitsery::ext::StdVariant{
                [](S& s, CreateDelete& create) { serialize(s, create); },
                [](S& s, ConnectDisconnect& edge) { serialize(s, edge); },
                [](S& s, Update& update) { serialize(s, update); },
                [](S& s, Confirm& confirm) { serialize(s, confirm.placeholder); }
            });
        }

        struct Message {
            const Payload payload;
            const Timestamp timestamp;

            Message() = delete;

            Message(Payload payload, Timestamp timestamp)
                : payload(std::move(payload))
                , timestamp(timestamp)
                {}
        };
        template <typename S>
        void serialize(S& s, Message& message) {
            serialize(s, message.payload);
            s.value8b(message.timestamp);
        };
    }
}
#endif //STRUCTUR3D_BASE_MESSAGE_H
