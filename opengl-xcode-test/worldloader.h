// worldloader.hpp

void loadWorld(const char * world_file_path, std::vector<std::string> &filenames, std::vector<glm::mat4> &translationMatrices, std::vector<glm::mat4> &scalingMatrices, std::vector<glm::mat4> &rotationMatrices, std::vector<int> &movableFlags, std::vector<bool> &splitMeshes, glm::vec3 &lightPositionWorld, glm::vec3 &camera, glm::vec3 &p, glm::vec3 &q, glm::vec3 &r);