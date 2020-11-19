//
// Created by Berci on 2020. 11. 19..
//

#ifndef STRUCTUR3D_BASE_MESSAGE_H
#define STRUCTUR3D_BASE_MESSAGE_H

#include <bitsery/bitsery.h>

#include <data/Base.h>
#include <data/Timestamp.h>
#include "Payload.h"

namespace S3D {
    struct Message {
        ID uid;
        Payload payload;
        Timestamp timestamp;
        bool deleted;
    };

    template <typename S>
    void serialize(S& s, Message& m) {
        s.writeBytes<sizeof(ID)>(m.uid);
        serialize(s, m.payload);
        s.value8b(m.timestamp);
        s.boolValue(m.deleted);
    };
}

#endif //STRUCTUR3D_BASE_MESSAGE_H
