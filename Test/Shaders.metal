//
//  Shaders.metal
//  Test
//
//  Created by Gustavo Binder on 02/07/24.
//

#include <metal_stdlib>

using namespace metal;

struct v2f
{
    float4 position [[position]];
};

v2f vertex vertexMain( uint vertexId [[vertex_id]],
                       device const float3* positions [[buffer(0)]])
{
    v2f o;
    o.position = float4( positions[ vertexId ], 1.0 );
    return o;
}

half4 fragment fragmentMain( v2f in [[stage_in]] )
{
    return half4( 1.0, 0.0, 0.0, 1.0 );
}
