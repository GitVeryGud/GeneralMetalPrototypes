//
//  Shaders.metal
//  Test
//
//  Created by Gustavo Binder on 02/07/24.
//

#include <metal_stdlib>
using namespace metal;

struct Uniforms {
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

struct v2f
{
    float4 position [[position]];
};

v2f vertex vertexMain( uint vertexId [[vertex_id]],
                       device const float3* positions [[buffer(0)]],
                       constant Uniforms &uniforms [[buffer(1)]])
{
    float4 fixedInPosition = float4( positions[ vertexId ], 1.0 );
    float4 position = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * fixedInPosition;
    
    v2f o;
    o.position = position;
    return o;
}

half4 fragment fragmentMain( v2f in [[stage_in]] )
{
    return half4( 1.0, 0.0, 0.0, 1.0 );
}

