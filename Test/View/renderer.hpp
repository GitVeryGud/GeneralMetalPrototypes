//
//  renderer.hpp
//  Test
//
//  Created by Gustavo Binder on 02/07/24.
//

#ifndef renderer_hpp
#define renderer_hpp

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <simd/simd.h>

class Renderer
{
public:
    Renderer( MTL::Device* pDevice );
    ~Renderer();
    void draw( MTK::View* pView );
    void buildBuffers();
    void buildShaders();
    
private:
    MTL::Device*                    _pDevice;
    MTL::CommandQueue*              _pCommandQueue;
    MTL::RenderPipelineState*       _pPSO;
    
    MTL::Buffer*                    _pVertexPositionsBuffer;
    MTL::Buffer*                    _pVertexColorsBuffer;
    MTL::Buffer*                    _pIndexBuffer;
    
    simd::float3*                   vertices    = nullptr;
    UInt32*                         indices     = nullptr;
    
    size_t                          numVertices = 0;
    size_t                          numIndices  = 0;
};

#endif /* renderer_hpp */

