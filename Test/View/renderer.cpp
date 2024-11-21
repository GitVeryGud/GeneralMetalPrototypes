//
//  renderer.cpp
//  Test
//
//  Created by Gustavo Binder on 02/07/24.
//

#include "renderer.hpp"

Renderer::Renderer( MTL::Device* pDevice )
: _pDevice( pDevice->retain() )
{
    _pCommandQueue = _pDevice->newCommandQueue();
    buildBuffers();
    buildShaders();
}

Renderer::~Renderer()
{
    _pCommandQueue->release();
    _pDevice->release();
    _pVertexPositionsBuffer->release();
    _pVertexColorsBuffer->release();
    _pPSO->release();
}

void Renderer::buildMatrices() {
    Transform transform = Transform();
    
    camera->position = vector3(0.f, 0.f, -5.f);

    uniforms->viewMatrix = camera->ViewMatrix();
    uniforms->projectionMatrix = camera->ProjectionMatrix();
}

void Renderer::buildBuffers() {
    const size_t NumVertices = 3;
    const size_t NumIndices  = 3;
    
    numVertices = NumVertices;
    numIndices = NumIndices;
    
    vertices = new simd::float3[NumVertices] {
        { 0,  1, 0},
        { 1, -1, 0},
        {-1, -1, 0}
        
    };
    
    indices = new UInt32[NumIndices] {
        0, 1, 2
    };
    
    const size_t SizeOfVertexPositionsBuffer = sizeof(simd::float3) * NumVertices;
    const size_t SizeOfIndexBuffer = sizeof(UInt32) * NumIndices;
    
    _pVertexPositionsBuffer = _pDevice->newBuffer(SizeOfVertexPositionsBuffer, MTL::ResourceStorageModeManaged);
    _pIndexBuffer = _pDevice->newBuffer(SizeOfIndexBuffer, MTL::ResourceStorageModeManaged);
    
    memcpy(_pVertexPositionsBuffer->contents(), vertices, SizeOfVertexPositionsBuffer);
    memcpy(_pIndexBuffer->contents(), indices, SizeOfIndexBuffer);
    
    _pVertexPositionsBuffer->didModifyRange(NS::Range::Make(0, _pVertexPositionsBuffer->length()));
    _pIndexBuffer->didModifyRange(NS::Range::Make(0, _pIndexBuffer->length()));
}

void Renderer::buildShaders() {
    using NS::StringEncoding::UTF8StringEncoding;

    NS::Error* pError = nullptr;
    MTL::Library* pLibrary = _pDevice->newDefaultLibrary();
    if ( !pLibrary )
    {
        __builtin_printf( "%s", pError->localizedDescription()->utf8String() );
        assert( false );
    }

    MTL::Function* pVertexFn = pLibrary->newFunction( NS::String::string("vertexMain", UTF8StringEncoding) );
    MTL::Function* pFragFn = pLibrary->newFunction( NS::String::string("fragmentMain", UTF8StringEncoding) );

    MTL::RenderPipelineDescriptor* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
    pDesc->setVertexFunction( pVertexFn );
    pDesc->setFragmentFunction( pFragFn );
    pDesc->colorAttachments()->object(0)->setPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB );
    pDesc->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);

    _pPSO = _pDevice->newRenderPipelineState( pDesc, &pError );
    if ( !_pPSO )
    {
        __builtin_printf( "%s", pError->localizedDescription()->utf8String() );
        assert( false );
    }

    pVertexFn->release();
    pFragFn->release();
    pDesc->release();
    pLibrary->release();
}

void Renderer::draw( MTK::View* pView )
{
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    
    angle += 1.f / (float)pView->preferredFramesPerSecond();;
    
    buildMatrices();
    
    Transform transform = Transform();
    transform.position = vector3(0.f, 0.f, -3.f);
    transform.rotation = vector3(0.f, angle * 60, 0.f);
    transform.scale = vector3(0.3f, 0.3f, 0.3f);
    
    uniforms->modelMatrix = transform.TransformMatrix();
    
    MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder( pRpd );
    
    pEnc->setRenderPipelineState(_pPSO);
    pEnc->setVertexBuffer(_pVertexPositionsBuffer, 0, 0);
    pEnc->setVertexBytes(uniforms, sizeof(Uniforms), NS::UInteger(1));
    
    pEnc->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, numIndices, MTL::IndexTypeUInt32, _pIndexBuffer, 0);
    
    pEnc->endEncoding();
    
    pCmd->presentDrawable( pView->currentDrawable() );
    pCmd->commit();

    pPool->release();
}
