#ifndef WORLDWRITER_H
#define WORLDWRITER_H

#include "model.h"

void writeWorld(std::vector<Model *> models, glm::vec3 lightPositionWorld, glm::vec3 camera, glm::vec3 p, glm::vec3 q, glm::vec3 r, std::vector<std::string> photoFilenames, std::vector<GLuint> photoTextures);

#endif