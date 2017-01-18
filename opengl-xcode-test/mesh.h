#ifndef MESH_H
#define MESH_H

#include <assimp/scene.h>
#include <GL/glew.h>

// Help with headers:
// http://www.acodersjourney.com/2016/05/top-10-c-header-file-mistakes-and-how-to-fix-them/

class Mesh
{
public:
    Mesh(aiMesh *inputMesh, int emptyCount);
    ~Mesh();
    std::vector<glm::vec3> *getVertices();
    std::vector<glm::vec2> *getUVs();
    std::vector<glm::vec3> *getNormals();
    std::vector<unsigned short> *getIndices();
    int getTextureIndex();
    GLuint getVertexBuffer();
    GLuint getUVBuffer();
    GLuint getNormalBuffer();
    GLuint getIndexBuffer();
    int getNumFaces();
    bool intersectsWithBoundingBox(std::vector<glm::vec3> vertices, int length, glm::mat4 objectToWorldspace);
    glm::vec3 getMinsObjectSpace();
    glm::vec3 getMaxesObjectSpace();
    std::vector<glm::vec3> getNormals(glm::mat4 objectToWorldspace);
    std::vector<glm::vec3> getPointsOnBoundingBoxFaces(glm::mat4 objectToWorldspace);
    std::vector<glm::vec3> getBoundingBox(glm::mat4 objectToWorldspace);
    
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<unsigned short> indices;
    int textureIndex;
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    GLuint indexbuffer;
    int numFaces;
    glm::vec3 minsObjectSpace;
    glm::vec3 maxesObjectSpace;
};

#endif