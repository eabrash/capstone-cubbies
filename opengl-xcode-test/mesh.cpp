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
    
    for (int i = 0; i < numVertices; i++)
    {
        vertices.push_back(glm::vec3(meshVertices[i].x, meshVertices[i].y, meshVertices[i].z));
        uvs.push_back(glm::vec2(meshTextures[i].x, meshTextures[i].y));
        normals.push_back(glm::vec3(meshNormals[i].x, meshNormals[i].y, meshNormals[i].z));
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