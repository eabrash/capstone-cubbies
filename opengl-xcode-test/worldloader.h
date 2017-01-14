// worldloader.hpp

void loadWorld(const char * world_file_path, std::vector<std::string> &filenames, std::vector<glm::mat4> &translationMatrices, std::vector<glm::mat4> &scalingMatrices, std::vector<glm::mat4> &rotationMatrices, std::vector<bool> &movableFlags);