#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include <GL/glew.h>


class Model
{
public:
    Model(std::string filename, glm::mat4 inputTranslation, glm::mat4 inputScale, glm::mat4 inputRotation, int movableStatus, bool splitMeshStatus);
    ~Model();
    std::vector<Mesh> * getMeshes();
    glm::mat4 getTranslation();
    glm::mat4 getRotation();
    glm::mat4 getScale();
    void setTranslation(glm::mat4 newTranslation);
    void setRotation(glm::mat4 newRotation);
    void setScale(glm::mat4 newScale);
    std::vector<GLuint> getTextures();
    void setTextures(std::vector<GLuint> newTextures);
    int getNumMeshes();
    bool collidedWithPlayer(glm::vec3 camera, glm::vec3 p, glm::vec3 q, glm::vec3 r);
    bool collidedWithObject(Model *object);
    int isMovable();
    void setMovable(int isMovable);
    bool modelMeshesSplit();
    glm::vec3 getMinsObjectSpace();
    glm::vec3 getMaxesObjectSpace();
    std::vector<glm::vec3> getBoundingBox();
    std::vector<glm::vec3> getNormals();
    std::vector<glm::vec3> getPointsOnBoundingBoxFaces();
    std::string getModelFile();
private:
    std::vector<Mesh> modelMeshes;
    glm::mat4 translation;
    glm::mat4 rotation;
    glm::mat4 scale;
    std::vector<GLuint> textures;
    int movable;
    bool splitMesh;
    glm::vec3 minsObjectSpace;
    glm::vec3 maxesObjectSpace;
    std::string modelFileName;
};

#endif