//#include <stdio.h>
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

std::string Model::getModelFile()
{
    return modelFileName;
}

std::vector<glm::vec3> Model::getNormals()
{
    std::vector<glm::vec3> planeNormals;
    
    glm::mat4 objectToWorldspace = translation * rotation * scale;
    
    glm::vec3 p1 = objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z,1);
    glm::vec3 p2 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z, 1);
    glm::vec3 p3 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, maxesObjectSpace.y, maxesObjectSpace.z, 1);
    
    planeNormals.push_back(glm::normalize(glm::cross((p2-p1), (p3-p2))));
    
    p1 = objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1);
    p2 = objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z,1);
    p3 = objectToWorldspace * glm::vec4(minsObjectSpace.x, maxesObjectSpace.y, maxesObjectSpace.z, 1);
    
    planeNormals.push_back(glm::normalize(glm::cross((p2-p1), (p3-p2))));
    
    p1 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1);
    p2 = objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1);
    p3 = objectToWorldspace * glm::vec4(minsObjectSpace.x, maxesObjectSpace.y, minsObjectSpace.z, 1);
    
    planeNormals.push_back(glm::normalize(glm::cross((p2-p1), (p3-p2))));
    
    p1 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z, 1);
    p2 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1);
    p3 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, maxesObjectSpace.y, minsObjectSpace.z, 1);
    
    planeNormals.push_back(glm::normalize(glm::cross((p2-p1), (p3-p2))));
    
    p1 = objectToWorldspace * glm::vec4(minsObjectSpace.x, maxesObjectSpace.y, maxesObjectSpace.z, 1);
    p2 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, maxesObjectSpace.y, maxesObjectSpace.z, 1);
    p3 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, maxesObjectSpace.y, minsObjectSpace.z, 1);
    
    planeNormals.push_back(glm::normalize(glm::cross((p2-p1), (p3-p2))));
    
    p1 = objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z, 1);
    p2 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1);
    p3 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z, 1);
    
    planeNormals.push_back(glm::normalize(glm::cross((p2-p1), (p3-p2))));

    return planeNormals;
}

std::vector<glm::vec3> Model::getPointsOnBoundingBoxFaces()
{
    std::vector<glm::vec3> pointsOnPlanes;
    
    glm::mat4 objectToWorldspace = translation * rotation * scale;
    
    pointsOnPlanes.push_back(objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z,1));
    
    pointsOnPlanes.push_back(objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1));
   
    pointsOnPlanes.push_back(objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1));
    
    pointsOnPlanes.push_back(objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z, 1));
    
    pointsOnPlanes.push_back(objectToWorldspace * glm::vec4(minsObjectSpace.x, maxesObjectSpace.y, maxesObjectSpace.z, 1));
    
    pointsOnPlanes.push_back(objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z, 1));

    return pointsOnPlanes;
}

std::vector<glm::vec3> Model::getBoundingBox()
{
    std::vector<glm::vec3> boundingBox;
    
    glm::vec3 mins = minsObjectSpace;
    glm::vec3 maxes = maxesObjectSpace;
    
    boundingBox.push_back(glm::vec3(mins.x, maxes.y, mins.z));
    boundingBox.push_back(glm::vec3(maxes.x, maxes.y, mins.z));
    boundingBox.push_back(glm::vec3(maxes.x, maxes.y, maxes.z));
    boundingBox.push_back(glm::vec3(mins.x, maxes.y, maxes.z));
    boundingBox.push_back(glm::vec3(maxes.x, mins.y, mins.z));
    boundingBox.push_back(glm::vec3(maxes.x, mins.y, maxes.z));
    boundingBox.push_back(glm::vec3(mins.x, mins.y, maxes.z));
    boundingBox.push_back(glm::vec3(mins.x, mins.y, mins.z));
    
    glm::mat4 objectToWorldspace = translation * rotation * scale;
    
    for (int i = 0; i < 8; i++)
    {
        boundingBox[i] = objectToWorldspace * glm::vec4(boundingBox[i], 1);
    }
    
    return boundingBox;
}

bool Model::collidedWithObject(Model *object)
{
    if (splitMesh) // If each mesh in the present model should be treated individually for collisions
    {
        if (object->splitMesh) // If each mesh in the passed-in model should also be treated individually...
        {
            std::vector<Mesh> *pObjectMeshes = object->getMeshes();
            
            // For each mesh in the passed-in object...
            
            for (int j = 0; j < pObjectMeshes->size(); j++)
            {
                
                std::vector<glm::vec3> boundingBox = (*pObjectMeshes)[j].getBoundingBox(object->getTranslation() * object->getRotation() * object->getScale());
                
                // for each mesh in the "this" object...
                
                for (int i = 0; i < modelMeshes.size(); i++)
                {
                    if (modelMeshes[i].intersectsWithBoundingBox(boundingBox, 8, translation*rotation*scale))
                    {
                        return true;
                    }
                }
            }
            
            // Flip it around...
            
            // For each mesh in the focal model
            for (int j = 0; j < modelMeshes.size(); j++)
            {
                
                std::vector<glm::vec3> boundingBox = modelMeshes[j].getBoundingBox(translation * rotation * scale);
                
                // for each mesh in the passed-in objectt
                
                for (int i = 0; i < pObjectMeshes->size(); i++)
                {
                    if ((*pObjectMeshes)[i].intersectsWithBoundingBox(boundingBox, 8, object->getTranslation()*object->getRotation()*object->getScale()))
                    {
                        return true;
                    }
                }
            }
        }
        else // If the passed-in object should be treated as a single block for collision purposes
        {
            // get the corners of the passed-in object's bounding box
            
            std::vector<glm::vec3> boundingBox = object->getBoundingBox();
            
            // for each mesh in the "this" object...
            
            for (int i = 0; i < modelMeshes.size(); i++)
            {
                if (modelMeshes[i].intersectsWithBoundingBox(boundingBox, 8, translation*rotation*scale))
                {
                    return true;
                }
            }
            
            // Flip it around...
            
            std::vector<glm::vec3> planeNormals = object->getNormals();
            std::vector<glm::vec3> pointsOnPlanes = object->getPointsOnBoundingBoxFaces();
            
            for (int i = 0; i < modelMeshes.size(); i++)
            {
                std::vector<glm::vec3> boundingBox = modelMeshes[i].getBoundingBox(translation*rotation*scale);
                
                for (int i = 0; i < 8; i++)
                {
                    if (glm::dot((boundingBox[i]-pointsOnPlanes[0]), planeNormals[0]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[1]), planeNormals[1]) <= 0 &&
                        glm::dot((boundingBox[i]-pointsOnPlanes[2]), planeNormals[2]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[3]), planeNormals[3]) <= 0 &&
                        glm::dot((boundingBox[i]-pointsOnPlanes[4]), planeNormals[4]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[5]), planeNormals[5]) <= 0)
                    {
                        return true;
                    }
                }

            }
        }
    }
    else // If the current model is to be treated as a single block in collisions
    {
        if (object->splitMesh) // If each mesh in the passed-in model should be treated individually...
        {
            std::vector<Mesh> *pObjectMeshes = object->getMeshes();
            
            std::vector<glm::vec3> planeNormals = getNormals();
            std::vector<glm::vec3> pointsOnPlanes = getPointsOnBoundingBoxFaces();
            
            // For each mesh in the passed-in object...
            
            for (int j = 0; j < pObjectMeshes->size(); j++)
            {
                std::vector<glm::vec3> boundingBox = (*pObjectMeshes)[j].getBoundingBox(object->getTranslation() * object->getRotation() * object->getScale());
                
                for (int i = 0; i < 8; i++)
                {
                    if (glm::dot((boundingBox[i]-pointsOnPlanes[0]), planeNormals[0]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[1]), planeNormals[1]) <= 0 &&
                        glm::dot((boundingBox[i]-pointsOnPlanes[2]), planeNormals[2]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[3]), planeNormals[3]) <= 0 &&
                        glm::dot((boundingBox[i]-pointsOnPlanes[4]), planeNormals[4]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[5]), planeNormals[5]) <= 0)
                    {
                        return true;
                    }
                }
            }
            
            // Flip it around...
            
            std::vector<glm::vec3> boundingBox = getBoundingBox();
            
            for (int j = 0; j < pObjectMeshes->size(); j++)
            {
                std::vector<glm::vec3> planeNormals = (*pObjectMeshes)[j].getNormals(object->getTranslation() * object->getRotation() * object->getScale());
                std::vector<glm::vec3> pointsOnPlanes = (*pObjectMeshes)[j].getPointsOnBoundingBoxFaces(object->getTranslation() * object->getRotation() * object->getScale());
                
                for (int i = 0; i < 8; i++)
                {
                    if (glm::dot((boundingBox[i]-pointsOnPlanes[0]), planeNormals[0]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[1]), planeNormals[1]) <= 0 &&
                        glm::dot((boundingBox[i]-pointsOnPlanes[2]), planeNormals[2]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[3]), planeNormals[3]) <= 0 &&
                        glm::dot((boundingBox[i]-pointsOnPlanes[4]), planeNormals[4]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[5]), planeNormals[5]) <= 0)
                    {
                        return true;
                    }
                }
            }
        }
        else // If the passed-in model should also be treated as a single block (typical case)
        {
            // get the corners of the passed-in object's bounding box
            std::vector<glm::vec3> boundingBox = object->getBoundingBox();
            
            // get normals and bounding box face reference points for the focal object
            std::vector<glm::vec3> planeNormals = getNormals();
            std::vector<glm::vec3> pointsOnPlanes = getPointsOnBoundingBoxFaces();
            
            for (int i = 0; i < 8; i++)
            {
                if (glm::dot((boundingBox[i]-pointsOnPlanes[0]), planeNormals[0]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[1]), planeNormals[1]) <= 0 &&
                    glm::dot((boundingBox[i]-pointsOnPlanes[2]), planeNormals[2]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[3]), planeNormals[3]) <= 0 &&
                    glm::dot((boundingBox[i]-pointsOnPlanes[4]), planeNormals[4]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[5]), planeNormals[5]) <= 0)
                {
                    return true;
                }
            }
            
            // Flip it around...
            
            // get the corners of the focal object's bounding box
            boundingBox = getBoundingBox();
            
            // get normals and bounding box face reference points for the focal object
            planeNormals = object->getNormals();
            pointsOnPlanes = object->getPointsOnBoundingBoxFaces();
            
            for (int i = 0; i < 8; i++)
            {
                if (glm::dot((boundingBox[i]-pointsOnPlanes[0]), planeNormals[0]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[1]), planeNormals[1]) <= 0 &&
                    glm::dot((boundingBox[i]-pointsOnPlanes[2]), planeNormals[2]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[3]), planeNormals[3]) <= 0 &&
                    glm::dot((boundingBox[i]-pointsOnPlanes[4]), planeNormals[4]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[5]), planeNormals[5]) <= 0)
                {
                    return true;
                }
            }
        }
    }
    
    // if we reached the end and did not detect collision with any mesh, there is no collision
    return false;
}

bool Model::modelMeshesSplit()
{
    return splitMesh;
}

glm::vec3 Model::getMinsObjectSpace()
{
    return minsObjectSpace;
}

glm::vec3 Model::getMaxesObjectSpace()
{
    return maxesObjectSpace;
}

int Model::isMovable()
{
    return movable;
}

void Model::setMovable(int isMovable)
{
    movable = isMovable;
}

bool Model::collidedWithPlayer(glm::vec3 camera, glm::vec3 p, glm::vec3 q, glm::vec3 r)
{
    // get the corners of the camera bounding box
    
    std::vector<glm::vec3> boundingBox;
    
    glm::vec3 mins = glm::vec3(-0.125, -4.6, -0.125);
    glm::vec3 maxes = glm::vec3(0.125, 0.125, 0.125);
    
    boundingBox.push_back(glm::vec3(mins.x, maxes.y, mins.z));
    boundingBox.push_back(glm::vec3(maxes.x, maxes.y, mins.z));
    boundingBox.push_back(glm::vec3(maxes.x, maxes.y, maxes.z));
    boundingBox.push_back(glm::vec3(mins.x, maxes.y, maxes.z));
    boundingBox.push_back(glm::vec3(maxes.x, mins.y, mins.z));
    boundingBox.push_back(glm::vec3(maxes.x, mins.y, maxes.z));
    boundingBox.push_back(glm::vec3(mins.x, mins.y, maxes.z));
    boundingBox.push_back(glm::vec3(mins.x, mins.y, mins.z));
    
    glm::vec3 pFlat = glm::normalize(glm::vec3(p.x, 0, p.z));
    glm::vec3 qFlat = glm::vec3(0,1,0);
    glm::vec3 rFlat = glm::normalize(glm::vec3(r.x, 0, r.z));
    
    glm::mat4 cameraToWorldspace;
    
    cameraToWorldspace[0] = glm::vec4(pFlat, 0);
    cameraToWorldspace[1] = glm::vec4(qFlat, 0);
    cameraToWorldspace[2] = glm::vec4(rFlat, 0);
    cameraToWorldspace[3] = glm::vec4(camera, 1);
    
    //std::cout << "CAMERA BOUNDING BOX:\n";
    
    for (int i = 0; i < 8; i++)
    {
        boundingBox[i] = cameraToWorldspace * glm::vec4(boundingBox[i].x, boundingBox[i].y, boundingBox[i].z, 1);
        //std::cout << boundingBox[i].x << " " << boundingBox[i].y << " " << boundingBox[i].z << "\n";
    }
    
    // for each mesh in the object
    
    if (splitMesh)
    {
        for (int i = 0; i < modelMeshes.size(); i++)
        {
            //std::cout << "WENT IN HERE" << "\n";
            
            // see if corners of camera bounding box penetrate the mesh bounding box (dot neg with all faces)
            // if any corner of the camera bounding box penetrates any mesh, return true (collision)
            
            if (modelMeshes[i].intersectsWithBoundingBox(boundingBox, 8, translation*rotation*scale))
            {
                return true;
            };
        }
    }
    else
    {        
        glm::vec3 planeNormals[6];
        glm::vec3 pointsOnPlanes[6];
        
        glm::mat4 objectToWorldspace = translation * rotation * scale;
        
        glm::vec3 p1 = objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z,1);
        glm::vec3 p2 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z, 1);
        glm::vec3 p3 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, maxesObjectSpace.y, maxesObjectSpace.z, 1);
        
        planeNormals[0] = glm::normalize(glm::cross((p2-p1), (p3-p2)));
        pointsOnPlanes[0] = p1;
        
        p1 = objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1);
        p2 = objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z,1);
        p3 = objectToWorldspace * glm::vec4(minsObjectSpace.x, maxesObjectSpace.y, maxesObjectSpace.z, 1);
        
        //    std::cout << p1.x << " " << p1.y << " " << p1.z << "\n";
        //    std::cout << p2.x << " " << p2.y << " " << p2.z << "\n";
        //    std::cout << p3.x << " " << p3.y << " " << p3.z << "\n";
        
        planeNormals[1] = glm::normalize(glm::cross((p2-p1), (p3-p2)));
        pointsOnPlanes[1] = p1;
        
        p1 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1);
        p2 = objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1);
        p3 = objectToWorldspace * glm::vec4(minsObjectSpace.x, maxesObjectSpace.y, minsObjectSpace.z, 1);
        
        //    std::cout << p1.x << " " << p1.y << " " << p1.z << "\n";
        //    std::cout << p2.x << " " << p2.y << " " << p2.z << "\n";
        //    std::cout << p3.x << " " << p3.y << " " << p3.z << "\n";
        
        planeNormals[2] = glm::normalize(glm::cross((p2-p1), (p3-p2)));
        pointsOnPlanes[2] = p1;
        
        p1 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z, 1);
        p2 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1);
        p3 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, maxesObjectSpace.y, minsObjectSpace.z, 1);
        
        //    std::cout << p1.x << " " << p1.y << " " << p1.z << "\n";
        //    std::cout << p2.x << " " << p2.y << " " << p2.z << "\n";
        //    std::cout << p3.x << " " << p3.y << " " << p3.z << "\n";
        
        planeNormals[3] = glm::normalize(glm::cross((p2-p1), (p3-p2)));
        pointsOnPlanes[3] = p1;
        
        p1 = objectToWorldspace * glm::vec4(minsObjectSpace.x, maxesObjectSpace.y, maxesObjectSpace.z, 1);
        p2 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, maxesObjectSpace.y, maxesObjectSpace.z, 1);
        p3 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, maxesObjectSpace.y, minsObjectSpace.z, 1);
        
        //    std::cout << p1.x << " " << p1.y << " " << p1.z << "\n";
        //    std::cout << p2.x << " " << p2.y << " " << p2.z << "\n";
        //    std::cout << p3.x << " " << p3.y << " " << p3.z << "\n";
        
        planeNormals[4] = glm::normalize(glm::cross((p2-p1), (p3-p2)));
        pointsOnPlanes[4] = p1;
        
        p1 = objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z, 1);
        p2 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1);
        p3 = objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z, 1);
        
        //    std::cout << p1.x << " " << p1.y << " " << p1.z << "\n";
        //    std::cout << p2.x << " " << p2.y << " " << p2.z << "\n";
        //    std::cout << p3.x << " " << p3.y << " " << p3.z << "\n";
        
        planeNormals[5] = glm::normalize(glm::cross((p2-p1), (p3-p2)));
        pointsOnPlanes[5] = p1;
        
        //std::cout << "PLANE NORMALS: \n";
        
        for (int i = 0; i < 6; i++)
        {
            //planeNormals[i] = objectToWorldspace * glm::vec4(planeNormals[i], 1);
            //std::cout << planeNormals[i].x << " " << planeNormals[i].y << " " << planeNormals[i].z << "\n";
        }
        
        for (int i = 0; i < 8; i++)
        {
            if (glm::dot((boundingBox[i]-pointsOnPlanes[0]), planeNormals[0]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[1]), planeNormals[1]) <= 0 &&
                glm::dot((boundingBox[i]-pointsOnPlanes[2]), planeNormals[2]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[3]), planeNormals[3]) <= 0 &&
                glm::dot((boundingBox[i]-pointsOnPlanes[4]), planeNormals[4]) <= 0 && glm::dot((boundingBox[i]-pointsOnPlanes[5]), planeNormals[5]) <= 0)
            {
                return true;
            }
        }
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

void Model::setTextures(std::vector<GLuint> newTextures)
{
    textures = newTextures;
}

Model::Model(std::string filename, glm::mat4 inputTranslation, glm::mat4 inputScale, glm::mat4 inputRotation, int movableStatus, bool splitMeshStatus)
{
    
    translation = inputTranslation;
    scale = inputScale;
    rotation = inputRotation;
    movable = movableStatus;
    splitMesh = splitMeshStatus;
    modelFileName = filename;
    
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
    
    for (int i = 0; i < modelMeshes.size(); i++)
    {
        glm::vec3 meshMins = modelMeshes[i].getMinsObjectSpace();
        glm::vec3 meshMaxes = modelMeshes[i].getMaxesObjectSpace();
        
        if (meshMins.x < minsObjectSpace.x)
        {
            minsObjectSpace.x = meshMins.x;
        }
        
        if (meshMins.y < minsObjectSpace.y)
        {
            minsObjectSpace.y = meshMins.y;
        }
        
        if (meshMins.z < minsObjectSpace.z)
        {
            minsObjectSpace.z = meshMins.z;
        }
        
        if (meshMaxes.x > maxesObjectSpace.x)
        {
            maxesObjectSpace.x = meshMaxes.x;
        }
        
        if (meshMaxes.y > maxesObjectSpace.y)
        {
            maxesObjectSpace.y = meshMaxes.y;
        }
        
        if (meshMaxes.z > maxesObjectSpace.z)
        {
            maxesObjectSpace.z = meshMaxes.z;
        }
    }
}