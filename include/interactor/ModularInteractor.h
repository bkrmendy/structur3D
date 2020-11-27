//
// Created by Berci on 2020. 11. 27..
//

#ifndef STRUCTUR3D_BASE_MODULARINTERACTOR_H
#define STRUCTUR3D_BASE_MODULARINTERACTOR_H

#include "Interactor.h"

#include <functional>
#include <utility>

namespace S3D {
    struct ModularInteractor: public Interactor {
        std::function<void(std::shared_ptr<Node> node, const ID &document, Timestamp timestamp)> create_;
        std::function<void(const ID& from, const ID& to, Timestamp timestamp)> connect_;
        std::function<void(const ID& entity, const Attribute& attribute, Timestamp timestamp)> upsert_;
        std::function<void(std::shared_ptr<Node> node, const ID& document, Timestamp timestamp)> remove_;
        std::function<void(const ID& from, const ID& to, Timestamp timestamp)> disconnect_;

        ModularInteractor(
                std::function<void(std::shared_ptr<Node> node, const ID &document, Timestamp timestamp)> create,
                std::function<void(const ID &, const ID &, Timestamp)> connect,
                std::function<void(const ID &, const Attribute &, Timestamp)> upsert,
                std::function<void(std::shared_ptr<Node>, const ID &, Timestamp)> remove,
                std::function<void(const ID &, const ID &, Timestamp)> disconnect)
            : create_(std::move(create))
            , connect_(std::move(connect))
            , upsert_(std::move(upsert))
            , remove_(std::move(remove))
            , disconnect_(std::move(disconnect))
            {}


        void create(std::shared_ptr<Node>, const ID& document, Timestamp timestamp) final;
        void connect(const ID& from, const ID& to, Timestamp timestamp) final;
        void upsert(const ID& entity, const Attribute& attribute, Timestamp timestamp) final;
        void remove(std::shared_ptr<Node> node, const ID& document, Timestamp timestamp) final;
        void disconnect(const ID& from, const ID& to, Timestamp timestamp) final;
    };
}

#endif //STRUCTUR3D_BASE_MODULARINTERACTOR_H
