// modelloader_assimp.cpp

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <vector>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Code modified from opengl-tutorials (http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/, see associated code for tutorial 09 in assimp version)
// and from http://www.assimp.org/lib_html/usage.html

int loadAssImp(std::string filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals, std::vector<unsigned short> &indices, std::vector<unsigned short> &verticesPerMesh, std::vector<unsigned short> &indicesPerMesh, std::vector<aiString> &texturePaths, std::vector<unsigned short> &textureIndices, std::vector<unsigned short> &texturesPerModel)
{
    Assimp::Importer importer;
    
    const aiScene *scene = importer.ReadFile(filename.c_str(), aiProcess_JoinIdenticalVertices);
    
    int numMeshes = scene->mNumMeshes;
    
    //    std::cout << "Num materials: " << scene->mNumMaterials << "\n";
    //    std::cout << "Num Meshes: " << numMeshes << "\n";
    //    std::cout << "Materials list: \n";
    
    std::vector<aiString> paths;
    
    int emptyCount = 0; // Number of empty textures encountered, usually at least one
    int numValidTextures = 0;
    
    for (int i = 0; i < scene->mNumMaterials; i++)
    {
        aiString filepath;
        int index = 0;
        
        // From http://www.lighthouse3d.com/cg-topics/code-samples/importing-3d-models-with-assimp/
        
        scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, index, &filepath);
        paths.push_back(filepath);
        
        //        std::cout << filepath.C_Str() << "\n";
        //        std::cout << index << "\n";
        
        if (filepath.length != 0)
        {
            texturePaths.push_back(filepath);
            numValidTextures++;
        }
        else
        {
            emptyCount++;
        }
    }
    
    texturesPerModel.push_back(numValidTextures);
    
    std::cout<< "Emptycount: " << emptyCount << "\n";
    
    for (int j = 0; j < numMeshes; j++)
    {
        aiMesh *mesh = scene->mMeshes[j];
        
        //std::cout << "Mesh " << j << ": material is " << mesh->mMaterialIndex << "\n";
        
        textureIndices.push_back(mesh->mMaterialIndex-emptyCount);
        
        //Get the pointers to the vertices, texture (uv) coords, and normals
        int numVertices = mesh->mNumVertices;
        aiVector3D *meshVertices = mesh->mVertices;
        aiVector3D *meshTextures = mesh->mTextureCoords[0];
        aiVector3D *meshNormals = mesh->mNormals;
        //aiVector3D *meshColors = mesh->mColors[0]; //We could also get colors if this had been in the .obj
        
        for (int i = 0; i < numVertices; i++)
        {
            vertices.push_back(glm::vec3(meshVertices[i].x, meshVertices[i].y, meshVertices[i].z));
            uvs.push_back(glm::vec2(meshTextures[i].x, meshTextures[i].y));
            normals.push_back(glm::vec3(meshNormals[i].x, meshNormals[i].y, meshNormals[i].z));
        }
        
        verticesPerMesh.push_back(numVertices);
        
        aiFace *meshFaces = mesh->mFaces;
        int numFaces = mesh->mNumFaces;
        
        for (int i = 0; i < numFaces; i++)
        {
            indices.push_back(meshFaces[i].mIndices[0]);
            indices.push_back(meshFaces[i].mIndices[1]);
            indices.push_back(meshFaces[i].mIndices[2]);
        }
        
        indicesPerMesh.push_back(numFaces);
    }
    
    return numMeshes;
}
