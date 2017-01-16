// worldloader.cpp
// Loads the world (model objects for a scene) from a text file
//
// Text file format:
//
// name_of_file1.obj
// 1.0 0.0 0.0 (translation, x y z)
// 1.0 1.0 1.0 (scaling, x y z)
// 90.0 0.0 1.0 0.1 (rotation, angle / axis (x y z))
// name_of_file2.obj
// 1.0 0.0 0.0 (translation, x y z)
// 1.0 1.0 1.0 (scaling, x y z)
// 90.0 0.0 1.0 0.1 (rotation, angle / axis (x y z))
// ...
//

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

// Load bitmap for texture. This method uses the FreeImage library for loading. FreeImage can load various
// image types, but this function is only intended for BMP files.

void loadWorld(const char * world_file_path, std::vector<std::string> &filenames, std::vector<glm::mat4> &translationMatrices, std::vector<glm::mat4> &scalingMatrices, std::vector<glm::mat4> &rotationMatrices, std::vector<int> &movableFlags, std::vector<bool> &splitMeshes)
{
    std::ifstream worldDataStream(world_file_path, std::ios::in); // Stream from file
    
    if(worldDataStream.is_open())
    {
        std::string line = "";
        int counter = 0;
        while(getline(worldDataStream, line)) // Keep getting lines from file while it has more
        {
            if (counter % 6 == 0)
            {
                filenames.push_back(line);
            }
            else if (counter % 6 == 1)
            {
                int begin = 0;
                int end = 0;
                float transforms [3];
                int transformsCounter = 0;
                
                for (int i = 0; i < line.length(); i++)
                {
                    if (isspace(line[i]))
                    {
                        end = i;
                        transforms[transformsCounter] = (float)::atof(line.substr(begin, end).c_str());
                        begin = i+1;
                        transformsCounter++;
                    }
                }
                
                transforms[transformsCounter] = (float)::atof(line.substr(begin, line.length()-1).c_str());
                
                translationMatrices.push_back(glm::translate(glm::vec3(transforms[0], transforms[1], transforms[2])));
            }
            else if (counter % 6 == 2)
            {
                int begin = 0;
                int end = 0;
                float transforms [3];
                int transformsCounter = 0;
                
                for (int i = 0; i < line.length(); i++)
                {
                    if (isspace(line[i]))
                    {
                        end = i;
                        transforms[transformsCounter] = (float)::atof(line.substr(begin, end).c_str());
                        begin = i+1;
                        transformsCounter++;
                    }
                }
                
                transforms[transformsCounter] = (float)::atof(line.substr(begin, line.length()-1).c_str());
                
                scalingMatrices.push_back(glm::scale(glm::vec3(transforms[0], transforms[1], transforms[2])));
                
                std::cout << "Scaling: " << transforms[0] << " " << transforms[1] << " " << transforms[2] << "\n";
            }
            else if (counter % 6 == 3)
            {
                int begin = 0;
                int end = 0;
                float transforms [4];
                int transformsCounter = 0;
                
                for (int i = 0; i < line.length(); i++)
                {
                    if (isspace(line[i]))
                    {
                        end = i;
                        transforms[transformsCounter] = (float)::atof(line.substr(begin, end).c_str());
                        begin = i+1;
                        transformsCounter++;
                    }
                }
                
                transforms[transformsCounter] = (float)::atof(line.substr(begin, line.length()-1).c_str());
                
                rotationMatrices.push_back(glm::rotate(glm::radians(transforms[0]), glm::vec3(transforms[1], transforms[2], transforms[3])));
                
                std::cout << "Rotation: " << transforms[0] << " " << transforms[1] << " " << transforms[2] << " " << transforms[3] << "\n";
            }
            else if (counter % 6 == 4)
            {
                if (line == "0")
                {
                    movableFlags.push_back(0);
                }
                else if (line == "1")
                {
                    movableFlags.push_back(1);
                }
                else if (line == "2")
                {
                      movableFlags.push_back(2);
                }
            }
            else if (counter % 6 == 5)
            {
                if (line == "0")
                {
                    splitMeshes.push_back(false);
                }
                else if (line == "1")
                {
                    splitMeshes.push_back(true);
                }
            }
            
            //std::cout << line << "\n";
            counter++;
        }
        worldDataStream.close();
    }
    else
    {
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", world_file_path);
    }
}