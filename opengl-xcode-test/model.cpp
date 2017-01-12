#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <vector>

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <FreeImage.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "model.h"
#include "imageloader.h"

int Model::getNumMeshes()
{
    return modelMeshes.size();
}

void Model::setTranslation(glm::mat4 newTranslation)
{
    translation = newTranslation;
}

void Model::setRotation(glm::mat4 newRotation)
{
    rotation = newRotation;
}

void Model::setScale(glm::mat4 newScale)
{
    scale = newScale;
}

std::vector<Mesh> * Model::getMeshes()
{
    return &modelMeshes;
}

glm::mat4 Model::getTranslation()
{
    return translation;
}

glm::mat4 Model::getScale()
{
    return scale;
}

glm::mat4 Model::getRotation()
{
    return rotation;
}

std::vector<GLuint> Model::getTextures()
{
    return textures;
}

Model::Model(std::string filename, glm::mat4 inputTranslation, glm::mat4 inputScale, glm::mat4 inputRotation)
{
    
    translation = inputTranslation;
    scale = inputScale;
    rotation = inputRotation;
    
    Assimp::Importer importer;
    
    const aiScene *scene = importer.ReadFile(filename.c_str(), aiProcess_JoinIdenticalVertices);
    
    int numMeshes = scene->mNumMeshes;
    
    int emptyCount = 0; // Number of empty textures encountered, usually at least one
    int numValidTextures = 0;
    
    std::vector<aiString> texturePaths;
    
    for (int i = 0; i < scene->mNumMaterials; i++)
    {
        aiString filepath;
        int index = 0;
        
        // From http://www.lighthouse3d.com/cg-topics/code-samples/importing-3d-models-with-assimp/
        
        scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, index, &filepath);
        //texturePaths.push_back(filepath);
        
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
    
    // This will give us back a bunch of OpenGL-internal texture handles for the BMPs we have loaded
    // (which are now OpenGL textures that we must call by these handles).
    for (int i = 0; i < texturePaths.size(); i++)
    {
        textures.push_back(loadBMP(texturePaths[i].C_Str()));
        std::cout << texturePaths[i].C_Str() << " \n";
    }
    
    //std::cout<< "Emptycount: " << emptyCount << "\n";
    
    for (int j = 0; j < numMeshes; j++)
    {
        aiMesh *assimpMesh = scene->mMeshes[j];
        
        //std::cout << "Mesh " << j << ": material is " << mesh->mMaterialIndex << "\n";
        
        Mesh *currentMesh = new Mesh(assimpMesh, emptyCount);
        
        modelMeshes.push_back(*currentMesh);
    }
}