// modelloader_assimp.h
// Header file for modelloader_assimp.cpp

int loadAssImp(std::string filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals, std::vector<unsigned short> &indices, std::vector<unsigned short> &verticesPerMesh, std::vector<unsigned short> &indicesPerMesh, std::vector<aiString> &texturePaths, std::vector<unsigned short> &textureIndices, std::vector<unsigned short> &texturesPerModel);