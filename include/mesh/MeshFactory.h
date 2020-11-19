//
// Created by Berci on 2020. 11. 19..
//

#ifndef STRUCTUR3D_BASE_MESHFACTORY_H
#define STRUCTUR3D_BASE_MESHFACTORY_H

#include <memory>

#include "Mesh.h"

namespace S3D {
    struct MeshFactory {
        virtual std::unique_ptr<Mesh> fromTree(const std::vector<Tree>&);
    };
}

#endif //STRUCTUR3D_BASE_MESHFACTORY_H
