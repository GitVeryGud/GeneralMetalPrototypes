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
    
    _pPSO = buildShaders("vertexMain", "fragmentPhong");
    _pPSSkyBox = buildShaders("vertexSkybox", "fragmentSkybox");
    
    directionalLight = new DirectionalLight();
    
    buildSphere();
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

void Renderer::buildSphere() {
    const float TAU = 6.28318548;
    
    UInt16 segments = 16;
    UInt16 rings = 8;
    float radius = 1;
    
    UInt16 verticesCount = segments * rings + 1;
    UInt16 indicesCount = ((rings - 2) * (segments - 1) * 6 + 2*(segments - 1) * 3 );
    
    simd::float3 positions[verticesCount];
    
    simd::float3 normals[verticesCount];
    
    UInt32 indices[indicesCount];
    
    for (UInt16 i = 1; i < rings; i++) {
        float valueI = i / (float) rings;
        float angleI = valueI * SNOWSOUP_PI_F;
        float y = cos(angleI);
        float mult = sin(angleI);
        
        for (UInt16 j = 0; j < segments; j++) {
            float valueJ = j / (float) (segments - 1);
            float angleJ = valueJ * TAU;
            float x = cos(angleJ) * mult;
            float z = -sin(angleJ) * mult;
            
            positions[j + (i - 1) * segments] = simd::float3 {x * radius, y * radius, z * radius};
            
            normals[j + (i - 1) * segments] = simd::float3 {x, y, z};
        }
    }
    
    UInt16 top = (UInt16) verticesCount - 2;
    positions[verticesCount - 2 ] = simd::float3 {0, radius, 0};
    normals[verticesCount - 2 ] = simd::float3 { 0, 1, 0 };
    
    UInt16 bot = (UInt16) verticesCount - 1;
    positions[verticesCount - 1 ] = simd::float3 {0, -radius, 0};
    normals[verticesCount - 1 ] = simd::float3 { 0, -1, 0 };
    
    for (UInt16 i = 0; i < rings - 2; i++) {
        for (UInt16 j = 0; j < segments - 1; j++) {
            UInt32 one = (i * segments) + j;
            UInt32 two = ((i + 1) * segments) + j;
            UInt32 three = ((i + 1) * segments) + j + 1;
            UInt32 four = (i * segments) + j + 1;
            
            int index = (i * (segments - 1) + j) * 6;
            
            indices[index] = one;
            indices[index + 1] = two;
            indices[index + 2] = three;
            
            indices[index + 3] = one;
            indices[index + 4] = three;
            indices[index + 5] = four;
        }
    }
    
    for (UInt16 i = 0; i < segments - 1; i++) {
        indices[indicesCount - 6 * (segments - 1) + i * 3] = i;
        indices[indicesCount - 6 * (segments - 1) + i * 3 + 1] = i + 1;
        indices[indicesCount - 6 * (segments - 1) + i * 3 + 2] = top;
    }
    
    for (UInt16 i = 0; i < segments - 1; i++) {
        UInt32 offset = (rings - 2) * segments;
        
        indices[indicesCount - 3 * (segments - 1) + i * 3 ] = offset + i;
        indices[indicesCount - 3 * (segments - 1) + i * 3 + 1] = bot;
        indices[indicesCount - 3 * (segments - 1) + i * 3 + 2] = offset + i + 1;
    }

    const size_t positionsDataSize = sizeof(positions);
    const size_t normalsDataSize = sizeof(normals);
    const size_t indexDataSize = sizeof(indices);
    
    numIndices = indicesCount;

    MTL::Buffer* pVertexPositionsBuffer = _pDevice->newBuffer(positionsDataSize, MTL::ResourceStorageModeManaged);
    MTL::Buffer* pVertexNormalsBuffer = _pDevice->newBuffer(normalsDataSize, MTL::ResourceStorageModeManaged);
    
    MTL::Buffer* index = _pDevice->newBuffer(indexDataSize, MTL::ResourceStorageModeManaged);

    _pVertexPositionsBuffer = pVertexPositionsBuffer;
    _pVertexNormalsBuffer = pVertexNormalsBuffer;
    
    _pIndexBuffer = index;

    memcpy( _pVertexPositionsBuffer->contents(), positions, positionsDataSize );
    memcpy( _pVertexNormalsBuffer->contents(), normals, normalsDataSize );
    memcpy( _pIndexBuffer->contents(), indices, indexDataSize );

    _pVertexPositionsBuffer->didModifyRange( NS::Range::Make( 0, _pVertexPositionsBuffer->length() ) );
    _pIndexBuffer->didModifyRange(NS::Range::Make(0, _pIndexBuffer->length()));
    _pVertexNormalsBuffer->didModifyRange(NS::Range::Make(0, _pVertexNormalsBuffer->length()));
}

void Renderer::buildSkybox() {
    skyUniforms->u_SkyColor = vector3(0.2f, 0.4f, 1.0f);
    skyUniforms->u_HorizonColor = vector3(0.3f, 0.7f, 0.9f);
    skyUniforms->u_GroundColor = vector3(0.2f, 0.6f, 0.2f);
    skyUniforms->u_SunSize = 0.03f;
    skyUniforms->u_LightColor = vector3(1.f, 1.f, 0.6f);
    skyUniforms->u_LightDir = vector3(0.f, 0.f, 0.f);
}

MTL::RenderPipelineState* Renderer::buildShaders(std::string vertex, std::string fragment) {
    using NS::StringEncoding::UTF8StringEncoding;

    NS::Error* pError = nullptr;
    MTL::Library* pLibrary = _pDevice->newDefaultLibrary();
    if ( !pLibrary )
    {
        __builtin_printf( "%s", pError->localizedDescription()->utf8String() );
        assert( false );
    }

    MTL::Function* pVertexFn = pLibrary->newFunction(NS::String::string(vertex.data(), UTF8StringEncoding));
    MTL::Function* pFragFn = pLibrary->newFunction( NS::String::string(fragment.data(), UTF8StringEncoding) );

    MTL::RenderPipelineDescriptor* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
    pDesc->setVertexFunction( pVertexFn );
    pDesc->setFragmentFunction( pFragFn );
    pDesc->colorAttachments()->object(0)->setPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm);
    pDesc->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);

    MTL::RenderPipelineState* pipelineState = _pDevice->newRenderPipelineState( pDesc, &pError );
    if ( !pipelineState )
    {
        __builtin_printf( "%s", pError->localizedDescription()->utf8String() );
        assert( false );
    }

    pVertexFn->release();
    pFragFn->release();
    pDesc->release();
    pLibrary->release();
    
    return pipelineState;
}

void Renderer::draw( MTK::View* pView )
{
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    
    angle += 1.f / (float)pView->preferredFramesPerSecond();;
    
    buildMatrices();
    buildSkybox();
    
    directionalLight->rotation.x = 45 * angle;
    skyUniforms->u_LightDir = directionalLight->Direction();
    
    Transform transform = Transform();
    
    MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder( pRpd );
    
    // Draw SKyboxSphere
    transform.position = camera->position;
    uniforms->modelMatrix = transform.TransformMatrix();
    pEnc->setRenderPipelineState(_pPSSkyBox);
    pEnc->setVertexBuffer(_pVertexPositionsBuffer, 0, 0);
    pEnc->setVertexBuffer(_pVertexNormalsBuffer, 0, 1);
    pEnc->setVertexBytes(uniforms, sizeof(Uniforms), NS::UInteger(2));
    pEnc->setFragmentBytes(skyUniforms, sizeof(SkyboxUniforms), NS::UInteger(0));
    
    pEnc->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, numIndices, MTL::IndexTypeUInt32, _pIndexBuffer, 0);
    
    // Draw Model
    transform.position = vector3(0.f, -0.3f, -3.f);
    transform.rotation = vector3(0.f, 0.f, 0.f);
    transform.scale = vector3(0.3f, 0.3f, 0.3f);
    uniforms->modelMatrix = transform.TransformMatrix();
    pEnc->setCullMode(MTL::CullModeBack);
    pEnc->setFrontFacingWinding(MTL::WindingCounterClockwise);
    pEnc->setRenderPipelineState(_pPSO);
    
    PhongUniforms* phongUniforms = new PhongUniforms;
    
    phongUniforms->u_AmbientLightColor = vector3(0.05f, 0.05f, 0.2f);
    phongUniforms->u_ViewPosition = camera->position;
    phongUniforms->u_LightDir = directionalLight->Direction();
    phongUniforms->u_LightColor = skyUniforms->u_LightColor;
    phongUniforms->u_SpecularIntensity = 0.1f;
    
    myModel->render(pEnc, uniforms, phongUniforms);
    
    pEnc->endEncoding();
    
    pCmd->presentDrawable( pView->currentDrawable() );
    pCmd->commit();

    pPool->release();
    
    delete phongUniforms;
}
