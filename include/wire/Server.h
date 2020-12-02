//
// Created by Berci on 2020. 11. 26..
//

#ifndef STRUCTUR3D_BASE_SERVER_H
#define STRUCTUR3D_BASE_SERVER_H

#include "data/Base.h"
#include "data/Timestamp.h"
#include "wire/Message.h"

namespace S3D {
    struct Server {
        virtual void sync(ID& document, Timestamp& timestamp, std::vector<Protocol::Message>& messages) = 0;
        virtual void receive(Protocol::Message& message) = 0;
        virtual void broadcast(Protocol::Message& message) = 0;
    };
}

#endif //STRUCTUR3D_BASE_SERVER_H
