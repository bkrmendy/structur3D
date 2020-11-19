//
//  Mesh.hpp
//  cross-platform-game iOS
//
//  Created by Berci on 2020. 11. 06..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <utility>
#include <vector>
#include <memory>
#include <simd/simd.h>

#include "data/Tree.h"
#include "types.h"

namespace S3D {
struct Mesh {
    const std::vector<S3DVertex> vertices;
    const std::vector<uint32_t> indices;

    Mesh(std::vector<S3DVertex> vertices, std::vector<uint32_t> indices)
        : vertices{std::move(vertices)}, indices{std::move(indices)} { }
};
}

#endif /* Mesh_hpp */
