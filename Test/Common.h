//
//  Common.h
//  Prototype 2
//
//  Created by Guilherme de Souza Barci on 14/11/24.
//

#ifndef Common_h
#define Common_h

typedef struct {
    matrix_float4x4 modelMatrix;
    matrix_float4x4 viewMatrix;
    matrix_float4x4 projectionMatrix;
} Uniforms;

typedef struct {
    vector_float3 u_LightDir;
    vector_float3 u_LightColor;
    vector_float3 u_SkyColor;
    vector_float3 u_HorizonColor;
    vector_float3 u_GroundColor;
    float u_SunSize;
} SkyboxUniforms;

#define SNOWSOUP_PI_F (3.14159265f)

#endif /* Common_h */
