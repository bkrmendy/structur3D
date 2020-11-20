//
// Created by Berci on 2020. 11. 19..
//

#ifndef STRUCTUR3D_BASE_NETWORKIMPL_H
#define STRUCTUR3D_BASE_NETWORKIMPL_H

#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>

#include "Network.h"


namespace S3D {
    using Buffer = std::vector<uint8_t>;
    using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;

    class NetworkImpl : public Network {
        const std::future<std::vector<Message>> sync(std::vector<Message> &) const final;
        void send(Message &message) const final;
    };
}

#endif //STRUCTUR3D_BASE_NETWORKIMPL_H
