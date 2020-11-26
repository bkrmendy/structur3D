//
// Created by Berci on 2020. 11. 19..
//

#ifndef STRUCTUR3D_BASE_CLIENTENDPOINT_H
#define STRUCTUR3D_BASE_CLIENTENDPOINT_H

#include <vector>

#include "Message.h"

namespace S3D {
    struct ClientEndpoint {
        virtual void connect(std::string& uri) const = 0;

        virtual void sync(std::vector<Protocol::Message>&) const = 0;
        virtual void send(Protocol::Message& message) const = 0;
        virtual void receive(Protocol::Message& message) const = 0;

        virtual ~ClientEndpoint() = default;
    };
}

#endif //STRUCTUR3D_BASE_CLIENTENDPOINT_H
