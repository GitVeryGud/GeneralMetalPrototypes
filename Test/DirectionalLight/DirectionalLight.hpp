//
//  DirectionalLight.hpp
//  Test
//
//  Created by Guilherme de Souza Barci on 21/11/24.
//

#ifndef DirectionalLight_hpp
#define DirectionalLight_hpp

#include <stdio.h>
#include <simd/simd.h>
#include "Common.h"

class DirectionalLight{
    public:
    inline DirectionalLight(){};
    vector_float3 rotation = vector3(0.f, 0.f, 0.f);
    vector_float3 color;
    vector_float3 Direction();
    
    simd_float4x4 RotationMatrix();
};

#endif /* DirectionalLight_hpp */
