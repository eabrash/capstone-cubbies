#include "model.h"

class Mesh
{
public:
    Mesh::Mesh(aiMesh *mesh, Model *parentModel, int emptyCount);
    ~Mesh();
    std::vector<glm::vec3> *getVertices();
    std::vector<glm::vec2> *getUVs();
    std::vector<glm::vec3> *getNormals();
    std::vector<unsigned short> *getIndices();
    int getTextureIndex();
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<unsigned short> indices;
    int textureIndex;
};