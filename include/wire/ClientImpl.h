//
// Created by Berci on 2020. 11. 19..
//

#ifndef STRUCTUR3D_BASE_CLIENTIMPL_H
#define STRUCTUR3D_BASE_CLIENTIMPL_H

#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>

#include <memory>
#include <interactor/Interactor.h>

#include "ClientEndpoint.h"


namespace S3D {
    using Buffer = std::vector<uint8_t>;
    using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;

    struct ClientImpl : public ClientEndpoint {
        std::shared_ptr<Interactor> interactor_;

        ClientImpl(std::shared_ptr<Interactor> interactor) : interactor_{std::move(interactor)} { }

        void process(Protocol::Message& message) const;

        void process(Protocol::CreateDelete& message, Timestamp timestamp) const;

        void connect(std::string& uri) const final;

        void sync(std::vector<Protocol::Message>& message) const final;
        void send(Protocol::Message& message) const final;
        void receive(Protocol::Message& message) const final;
    };
}

#endif //STRUCTUR3D_BASE_CLIENTIMPL_H
