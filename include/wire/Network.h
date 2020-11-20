//
// Created by Berci on 2020. 11. 19..
//

#ifndef STRUCTUR3D_BASE_NETWORK_H
#define STRUCTUR3D_BASE_NETWORK_H

#include <future>
#include <vector>

#include "Message.h"

namespace S3D {
    struct Network {
        virtual const std::future<std::vector<Message>> sync(std::vector<Message>&) const = 0;
        virtual void send(Message& message) const = 0;
        virtual ~Network() = default;
    };
}

#endif //STRUCTUR3D_BASE_NETWORK_H
