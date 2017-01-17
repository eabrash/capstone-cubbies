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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

//class Mesh
//{
//public:
//    Mesh(aiMesh *inputMesh, int emptyCount);
//    ~Mesh();
//    std::vector<glm::vec3> *getVertices();
//    std::vector<glm::vec2> *getUVs();
//    std::vector<glm::vec3> *getNormals();
//    std::vector<unsigned short> *getIndices();
//    int getTextureIndex();
//    GLuint getVertexBuffer();
//    GLuint getUVBuffer();
//    GLuint getNormalBuffer();
//    GLuint getIndexBuffer();
//    int getNumFaces();
//    bool intersectsWithBoundingBox(glm::vec3 *vertices, int length);
//private:
//    std::vector<glm::vec3> vertices;
//    std::vector<glm::vec2> uvs;
//    std::vector<glm::vec3> normals;
//    std::vector<unsigned short> indices;
//    int textureIndex;
//    GLuint vertexbuffer;
//    GLuint uvbuffer;
//    GLuint normalbuffer;
//    GLuint indexbuffer;
//    int numFaces;
//    glm::vec3 minsObjectSpace;
//    glm::vec3 maxesObjectSpace;
//};


glm::vec3 Mesh::getMinsObjectSpace()
{
    return minsObjectSpace;
}

glm::vec3 Mesh::getMaxesObjectSpace()
{
    return maxesObjectSpace;
}

std::vector<glm::vec3> Mesh::getNormals(glm::mat4 objectToWorldspace)
{
    std::vector<glm::vec3> planeNormals;
    
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

std::vector<glm::vec3> Mesh::getPointsOnBoundingBoxFaces(glm::mat4 objectToWorldspace)
{
    std::vector<glm::vec3> pointsOnPlanes;
    
    pointsOnPlanes.push_back(objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z,1));
    
    pointsOnPlanes.push_back(objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1));
    
    pointsOnPlanes.push_back(objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, minsObjectSpace.z, 1));
    
    pointsOnPlanes.push_back(objectToWorldspace * glm::vec4(maxesObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z, 1));
    
    pointsOnPlanes.push_back(objectToWorldspace * glm::vec4(minsObjectSpace.x, maxesObjectSpace.y, maxesObjectSpace.z, 1));
    
    pointsOnPlanes.push_back(objectToWorldspace * glm::vec4(minsObjectSpace.x, minsObjectSpace.y, maxesObjectSpace.z, 1));
    
    return pointsOnPlanes;
}

std::vector<glm::vec3> Mesh::getBoundingBox(glm::mat4 objectToWorldspace)
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
    
    for (int i = 0; i < 8; i++)
    {
        boundingBox[i] = objectToWorldspace * glm::vec4(boundingBox[i], 1);
    }
    
    return boundingBox;
}

bool Mesh::intersectsWithBoundingBox(std::vector<glm::vec3> vertices, int length, glm::mat4 objectToWorldspace)
{
    glm::vec3 planeNormals[6];
    glm::vec3 pointsOnPlanes[6];
    
//    glm::vec3 minsWorldspace = objectToWorldspace * glm::vec4(minsObjectSpace,0);
//    glm::vec3 maxesWorldspace = objectToWorldspace * glm::vec4(maxesObjectSpace,0);
    
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
    
    for (int i = 0; i < length; i++)
    {
        if (glm::dot((vertices[i]-pointsOnPlanes[0]), planeNormals[0]) <= 0 && glm::dot((vertices[i]-pointsOnPlanes[1]), planeNormals[1]) <= 0 &&
            glm::dot((vertices[i]-pointsOnPlanes[2]), planeNormals[2]) <= 0 && glm::dot((vertices[i]-pointsOnPlanes[3]), planeNormals[3]) <= 0 &&
            glm::dot((vertices[i]-pointsOnPlanes[4]), planeNormals[4]) <= 0 && glm::dot((vertices[i]-pointsOnPlanes[5]), planeNormals[5]) <= 0)
        {
            return true;
        }
    }
    
    return false;
}

int Mesh::getNumFaces()
{
    return numFaces;
}

GLuint Mesh::getIndexBuffer()
{
    return indexbuffer;
}

GLuint Mesh::getVertexBuffer()
{
    return vertexbuffer;
}

GLuint Mesh::getUVBuffer()
{
    return uvbuffer;
}

GLuint Mesh::getNormalBuffer()
{
    return normalbuffer;
}

Mesh::Mesh(aiMesh *mesh, int emptyCount)
{
    textureIndex = (mesh->mMaterialIndex-emptyCount);
    
    //Get the pointers to the vertices, texture (uv) coords, and normals
    int numVertices = mesh->mNumVertices;
    aiVector3D *meshVertices = mesh->mVertices;
    aiVector3D *meshTextures = mesh->mTextureCoords[0];
    aiVector3D *meshNormals = mesh->mNormals;
    //aiVector3D *meshColors = mesh->mColors[0]; //We could also get colors if this had been in the .obj
    
    minsObjectSpace = glm::vec3(meshVertices[0].x, meshVertices[0].y, meshVertices[0].z);
    maxesObjectSpace = glm::vec3(meshVertices[0].x, meshVertices[0].y, meshVertices[0].z);
    
    for (int i = 0; i < numVertices; i++)
    {
        vertices.push_back(glm::vec3(meshVertices[i].x, meshVertices[i].y, meshVertices[i].z));
        uvs.push_back(glm::vec2(meshTextures[i].x, meshTextures[i].y));
        normals.push_back(glm::vec3(meshNormals[i].x, meshNormals[i].y, meshNormals[i].z));
        
        if (meshVertices[i].x < minsObjectSpace.x)
        {
            minsObjectSpace.x = meshVertices[i].x;
        }
 
        if (meshVertices[i].y < minsObjectSpace.y)
        {
            minsObjectSpace.y = meshVertices[i].y;
        }

        if (meshVertices[i].z < minsObjectSpace.z)
        {
            minsObjectSpace.z = meshVertices[i].z;
        }
        
        if (meshVertices[i].x > maxesObjectSpace.x)
        {
            maxesObjectSpace.x = meshVertices[i].x;
        }
        
        if (meshVertices[i].y > maxesObjectSpace.y)
        {
            maxesObjectSpace.y = meshVertices[i].y;
        }
        
        if (meshVertices[i].z > maxesObjectSpace.z)
        {
            maxesObjectSpace.z = meshVertices[i].z;
        }
    }
    
    aiFace *meshFaces = mesh->mFaces;
    numFaces = mesh->mNumFaces;
    
    for (int i = 0; i < numFaces; i++)
    {
        indices.push_back(meshFaces[i].mIndices[0]);
        indices.push_back(meshFaces[i].mIndices[1]);
        indices.push_back(meshFaces[i].mIndices[2]);
    }
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(float)*3, &vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(float)*2, &uvs[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(float)*3, &normals[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &indexbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces*sizeof(unsigned short)*3, &indices[0], GL_STATIC_DRAW);
}

Mesh::~Mesh()
{
    std::cout << "In Mesh class destructor\n";
}

std::vector<glm::vec3> * Mesh::getVertices()
{
    return &vertices;
}

std::vector<glm::vec2> * Mesh::getUVs()
{
    return &uvs;
}

std::vector<glm::vec3> * Mesh::getNormals()
{
    return &normals;
}

std::vector<unsigned short> * Mesh::getIndices()
{
    return &indices;
}

int Mesh::getTextureIndex()
{
    return textureIndex;
}