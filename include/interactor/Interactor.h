//
// Created by Berci on 2020. 11. 22..
//

#ifndef STRUCTUR3D_BASE_INTERACTOR_H
#define STRUCTUR3D_BASE_INTERACTOR_H

#include <memory>

#include "data/Node.h"
#include "data/Edge.h"

namespace S3D {
    struct Interactor {
        virtual void create(std::shared_ptr<Node>, const ID& document) = 0;
        virtual void connect(const ID& from, const ID& to) = 0;
        virtual void update(std::shared_ptr <Node> node) = 0;
        virtual void remove(std::shared_ptr<Node> node, const ID& document) = 0;
        virtual void disconnect(const ID& from, const ID& to) = 0;

        virtual ~Interactor() = default;
    };

}

#endif //STRUCTUR3D_BASE_INTERACTOR_H
