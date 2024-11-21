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
    
    myModel = new Model("model", _pDevice);
    myModel->importModel("Models/character.obj");
    
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
    pDesc->colorAttachments()->object(0)->setPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm);
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
    transform.position = vector3(0.f, -0.3f, -3.f);
    transform.rotation = vector3(0.f, 0.f, 0.f);
    transform.scale = vector3(0.3f, 0.3f, 0.3f);
    
    uniforms->modelMatrix = transform.TransformMatrix();
    
    MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder( pRpd );
    
    pEnc->setCullMode(MTL::CullModeBack);
    pEnc->setFrontFacingWinding(MTL::WindingCounterClockwise);
    pEnc->setRenderPipelineState(_pPSO);
    
    myModel->render(pEnc, uniforms);
    
    pEnc->endEncoding();
    
    pCmd->presentDrawable( pView->currentDrawable() );
    pCmd->commit();

    pPool->release();
}
