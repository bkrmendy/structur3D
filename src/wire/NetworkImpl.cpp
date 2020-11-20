//
// Created by Berci on 2020. 11. 19..
//

#include <bitsery/bitsery.h>

#include <wire/NetworkImpl.h>

namespace S3D {
    void NetworkImpl::send(Message &message) const {
        Buffer buffer;

        auto writtenSize = bitsery::quickSerialization<OutputAdapter>(buffer, message);
    }
}