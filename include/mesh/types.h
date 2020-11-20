//
//  types.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 04..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef types_h
#define types_h

#include <simd/simd.h>

namespace S3D {
    using Vertex = struct {
        vector_float4 position;
        vector_float4 normal;
    };

    using Uniforms = struct {
        matrix_float4x4 modelViewProjectionMatrix;
        matrix_float4x4 modelViewMatrix;
        matrix_float3x3 normalMatrix;
    };
}


#endif /* types_h */
