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
    bool intersectsWithBoundingBox(glm::vec3 *vertices, int length, glm::mat4 objectToWorldspace);
    glm::vec3 getMinsObjectSpace();
    glm::vec3 getMaxesObjectSpace();
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
