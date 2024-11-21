//
//  Transform.hpp
//  Test
//
//  Created by Guilherme de Souza Barci on 28/06/24.
//

#ifndef Transform_hpp
#define Transform_hpp

#include <cassert>

#include <stdio.h>
#include <simd/simd.h>
#include "Common.h"

class Transform{
    public:
    inline Transform(){};
    vector_float3 position = vector3(0.f, 0.f, 0.f);
    vector_float3 rotation = vector3(0.f, 0.f, 0.f);
    vector_float3 scale = vector3(1.f, 1.f, 1.f);
    
    simd_float4x4 RotationMatrix();
    simd_float4x4 TransformMatrix();
    
    vector_float3 Forward();
    vector_float3 Right();
    vector_float3 Up();
};

#endif /* Transform_hpp */
