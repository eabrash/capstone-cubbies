class Model
{
public:
    Model(std::string filename, glm::mat4 inputTranslation, glm::mat4 inputScale, glm::mat4 inputRotation, bool movableStatus);
    ~Model();
    std::vector<Mesh> * getMeshes();
    glm::mat4 getTranslation();
    glm::mat4 getRotation();
    glm::mat4 getScale();
    void setTranslation(glm::mat4 newTranslation);
    void setRotation(glm::mat4 newRotation);
    void setScale(glm::mat4 newScale);
    std::vector<GLuint> getTextures();
    int getNumMeshes();
    bool collidedWithPlayer(glm::vec3 camera, glm::vec3 p, glm::vec3 q, glm::vec3 r);
    bool isMovable();
    void setMovable(bool isMovable);
private:
    std::vector<Mesh> modelMeshes;
    glm::mat4 translation;
    glm::mat4 rotation;
    glm::mat4 scale;
    std::vector<GLuint> textures;
    bool movable;
};
