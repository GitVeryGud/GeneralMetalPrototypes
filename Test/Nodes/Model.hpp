//
//  Model.hpp
//  Test
//
//  Created by Gustavo Binder on 26/06/24.
//

#ifndef Model_hpp
#define Model_hpp

#include <cassert>
#include <iostream>

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <simd/simd.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Mesh.hpp"
#include "../Common.h"
#include "Texture/Texture.hpp"

class Model {
public:
    Model( std::string name, MTL::Device* pDevice );
    void render( MTL::RenderCommandEncoder* pCommandEncoder, Uniforms* uniforms );
    void setModel();
    void importModel(const std::string pFile);
    void buildBuffers();
    
private:
    const aiScene*  scene        = nullptr;
    
    MTL::Buffer*    vertexBuffer = nullptr;
    MTL::Buffer*    textureBuffer = nullptr;
    
    simd::float2* textureVertices = nullptr;
    simd::float3*   vertices      = nullptr;
    Mesh**           meshes       = nullptr;
    
    size_t          vertexAmount = 0;
    size_t          meshAmount   = 0;
    size_t          numTextVertices = 0;
    
    Texture* texture = nullptr;
    
    MTL::Device*    pDevice      = nullptr;
};

#endif /* Model_hpp */
