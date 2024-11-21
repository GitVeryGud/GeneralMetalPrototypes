//
//  Model.cpp
//  Test
//
//  Created by Gustavo Binder on 26/06/24.
//

#include "Model.hpp"

Model::Model( std::string name, MTL::Device* pDevice) {
    this->pDevice = pDevice;
}

void Model::render( MTL::RenderCommandEncoder* pCommandEncoder, Uniforms* uniforms ) {
    
    if (vertexBuffer == nullptr)
        return;
    
    pCommandEncoder->setVertexBuffer(vertexBuffer, 0, 0);
    pCommandEncoder->setVertexBytes(uniforms, sizeof(Uniforms), 1);
    
    for (int i = 0; i < meshAmount; i++) {
        Mesh* currentMesh = meshes[i];
        
        pCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, currentMesh->indexAmount, MTL::IndexTypeUInt32, currentMesh->indexBuffer, NS::UInteger(0));
    }
}

void Model::importModel(const std::string pFile) {
    
    Assimp::Importer importer;
    
    scene = importer.ReadFile(
                                             pFile,
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate);
    
    if (scene == nullptr) {
        size_t pfound = pFile.find_last_of('/');
        std::cout << "Failed to import " << pFile.substr(pfound + 1) << "\n";
        assert(false);
    }
    
    setModel();
}

void Model::setModel() {
    if (scene == nullptr)
        return;
    
    // CHECK IF MODEL ALREADY HAS MESHES
    // DELETES THEM IF IT DOES
    meshAmount = scene->mNumMeshes;
    if (meshes != nullptr)
        delete meshes;
    meshes = new Mesh*[meshAmount];
    
    // CHECK IF MODEL ALREADY HAS VERTICES
    // DELETES THEM IF IT DOES
    if (vertices != nullptr)
        delete vertices;
    for (int i = 0; i < scene->mNumMeshes; i++)
        vertexAmount += scene->mMeshes[i]->mNumVertices;
    vertices = new simd::float3[vertexAmount];
    
    size_t vertexTrack = 0;
    
    // ---------- BEGIN WRITING THE MESHES -----------
    for (int i = 0; i < scene->mNumMeshes; i++) {
        Mesh* newMesh = new Mesh;
        aiMesh* currentMesh = scene->mMeshes[i];
        
        newMesh->indexAmount = currentMesh->mNumFaces * 3;
        
        if (newMesh->indices != nullptr)
            delete newMesh->indices;
        
        newMesh->indices = new UInt32[newMesh->indexAmount];
        
        // ADD VERTICES
        for (int j = 0; j < currentMesh->mNumVertices; j++) {
            aiVector3D currentAiVertex = currentMesh->mVertices[j];
            
            vertices[j + vertexTrack].x = currentAiVertex.x;
            vertices[j + vertexTrack].y = currentAiVertex.y;
            vertices[j + vertexTrack].z = currentAiVertex.z;
        }
        
        // ADD INDICES
        for (int j = 0; j < currentMesh->mNumFaces; j++) {
            aiFace* currentFace = &currentMesh->mFaces[j];
            
            for (int k = 0; k < 3; k++)
                newMesh->indices[j * 3 + k] = currentFace->mIndices[k] + (UInt32)vertexTrack;
        }
        
        vertexTrack += currentMesh->mNumVertices;
        
        // ADD MESH
        meshes[i] = newMesh;
    }
    
    buildBuffers();
}

void Model::buildBuffers() {
    const size_t sizeOfVertexBuffer = vertexAmount * sizeof(simd::float3);
    
    vertexBuffer = pDevice->newBuffer(sizeOfVertexBuffer, MTL::ResourceStorageModeManaged);
    
    memcpy(vertexBuffer->contents(), vertices, sizeOfVertexBuffer);
    
    for (int i = 0; i < meshAmount; i++) {
        Mesh* currentMesh = meshes[i];
        const size_t sizeOfIndexBuffer = currentMesh->indexAmount * sizeof(UInt32);
        
        currentMesh->indexBuffer = pDevice->newBuffer(sizeOfIndexBuffer, MTL::ResourceStorageModeManaged);
        memcpy(currentMesh->indexBuffer->contents(), currentMesh->indices, sizeOfIndexBuffer);
        
        currentMesh->indexBuffer->didModifyRange(NS::Range::Make( 0, currentMesh->indexBuffer->length() ));
    }
    
    vertexBuffer->didModifyRange(NS::Range::Make(0, vertexBuffer->length()));
}
