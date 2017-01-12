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
};
