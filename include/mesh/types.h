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

typedef struct {
    vector_float4 position;
    vector_float4 normal;
} S3DVertex;

typedef struct {
    matrix_float4x4 modelViewProjectionMatrix;
    matrix_float4x4 modelViewMatrix;
    matrix_float3x3 normalMatrix;
} S3DUniforms;


#endif /* types_h */
