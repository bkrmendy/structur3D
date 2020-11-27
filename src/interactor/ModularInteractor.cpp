//
// Created by Berci on 2020. 11. 27..
//

#include "interactor/ModularInteractor.h"

namespace S3D {

    void ModularInteractor::create(std::shared_ptr<Node> node, const ID &document, Timestamp timestamp) {
        this->create_(node, document, timestamp);
    }

    void ModularInteractor::connect(const ID &from, const ID &to, Timestamp timestamp) {
        this->connect_(from, to, timestamp);
    }

    void ModularInteractor::upsert(const ID &entity, const Attribute &attribute, Timestamp timestamp) {
        this->upsert_(entity, attribute, timestamp);
    }

    void ModularInteractor::remove(std::shared_ptr<Node> node, const ID &document, Timestamp timestamp) {
        this->remove_(node, document, timestamp);
    }

    void ModularInteractor::disconnect(const ID &from, const ID &to, Timestamp timestamp) {
        this->disconnect_(from, to, timestamp);
    }
}
