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

//class Model
//{
//public:
//    Model(std::string filename, glm::mat4 inputTranslation, glm::mat4 inputScale, glm::mat4 inputRotation);
//    ~Model();
//    std::vector<Mesh> * getMeshes();
//    glm::mat4 getTranslation();
//    glm::mat4 getRotation();
//    glm::mat4 getScale();
//    void setTranslation(glm::mat4 newTranslation);
//    void setRotation(glm::mat4 newRotation);
//    void setScale(glm::mat4 newScale);
//    std::vector<GLuint> getTextures();
//    int getNumMeshes();
//    bool collidedWith(glm::vec3 camera, glm::vec3 p, glm::vec3 q, glm::vec3 r);
//private:
//    std::vector<Mesh> modelMeshes;
//    glm::mat4 translation;
//    glm::mat4 rotation;
//    glm::mat4 scale;
//    std::vector<GLuint> textures;
//};


bool Model::collidedWithPlayer(glm::vec3 camera, glm::vec3 p, glm::vec3 q, glm::vec3 r)
{
    // get the corners of the camera bounding box
    
    glm::vec3 boundingBox[8];
    
    glm::vec3 mins = glm::vec3(-1, -5, -0.5);
    glm::vec3 maxes = glm::vec3(1, 1, 0.5);
    
    boundingBox[0] = glm::vec3(mins.x, maxes.y, mins.z);
    boundingBox[1] = glm::vec3(maxes.x, maxes.y, mins.z);
    boundingBox[2] = glm::vec3(maxes.x, maxes.y, maxes.z);
    boundingBox[3] = glm::vec3(mins.x, maxes.y, maxes.z);
    boundingBox[4] = glm::vec3(maxes.x, mins.y, mins.z);
    boundingBox[5] = glm::vec3(maxes.x, mins.y, maxes.z);
    boundingBox[6] = glm::vec3(mins.x, mins.y, maxes.z);
    boundingBox[7] = glm::vec3(mins.x, mins.y, mins.z);
    
    // for each mesh in the object
    
    for (int i = 0; i < modelMeshes.size(); i++)
    {
        
        // see if corners of camera bounding box penetrate the mesh bounding box (dot neg with all faces)
        // if any corner of the camera bounding box penetrates any mesh, return true (collision)
        
        if (modelMeshes[i].intersectsWithBoundingBox(boundingBox, 8, translation*rotation*scale))
        {
            return true;
        };
            
        // if any corner of the camera bounding box penetrates any mesh of the model, return true (collision)
    }
    
    // if we reached the end and did not detect collision with any mesh, there is no collision
    return false;
}

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