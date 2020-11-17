//
//  Mesh.hpp
//  cross-platform-game iOS
//
//  Created by Berci on 2020. 11. 06..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <vector>
#include <memory>
#include <simd/simd.h>

#include "data/Tree.h"
#include "types.h"

namespace S3D {
class Mesh {
public:
    const std::vector<S3DVertex> vertices;
    const std::vector<uint32_t> indices;

    static std::unique_ptr<Mesh> fromTree(const Tree& tree);

    Mesh(std::vector<S3DVertex> vertices, std::vector<uint32_t> indices)
        : vertices{vertices}, indices{indices} { }
};
}

#endif /* Mesh_hpp */
