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
// image types, but this function is only intended for BMP files. Based on file loading code from opengl-
// tutorials.

void loadWorld(const char * world_file_path, std::vector<std::string> &filenames, std::vector<glm::mat4> &translationMatrices, std::vector<glm::mat4> &scalingMatrices, std::vector<glm::mat4> &rotationMatrices, std::vector<int> &movableFlags, std::vector<bool> &splitMeshes, std::vector<glm::vec3> &lightPositionsWorld, std::vector<float> &lightPowers, glm::vec3 &camera, glm::vec3 &p, glm::vec3 &q, glm::vec3 &r, std::vector<std::string> &photoNames)
{
    std::ifstream worldDataStream(world_file_path, std::ios::in); // Stream from file
    
    if(worldDataStream.is_open())
    {
        std::string line = "";
        
        //Get number of light sources
        
        getline(worldDataStream, line);
        int numLights = (int)::atof(line.c_str());
        
        for (int i = 0; i < numLights; i++)
        {
            //Get light position
            getline(worldDataStream, line);
            
            int begin = 0;
            int end = 0;
            float light [3];
            int lightCounter = 0;
            
            for (int i = 0; i < line.length(); i++)
            {
                if (isspace(line[i]))
                {
                    end = i;
                    light[lightCounter] = (float)::atof(line.substr(begin, end).c_str());
                    begin = i+1;
                    lightCounter++;
                }
            }
            
            light[lightCounter] = (float)::atof(line.substr(begin, line.length()-1).c_str());
            
            lightPositionsWorld.push_back(glm::vec3(light[0], light[1], light[2]));
            
            //Get light power
            getline(worldDataStream, line);
            
            begin = 0;
            end = 0;
            float lightPower = (float)::atof(line.c_str());
            lightPowers.push_back(lightPower);
        }
        
        //Get camera position
        getline(worldDataStream, line);
        
        int begin = 0;
        int end = 0;
        float cameraArray[3];
        int cameraCounter = 0;
        
        for (int i = 0; i < line.length(); i++)
        {
            if (isspace(line[i]))
            {
                end = i;
                cameraArray[cameraCounter] = (float)::atof(line.substr(begin, end).c_str());
                begin = i+1;
                cameraCounter++;
            }
        }
        
        cameraArray[cameraCounter] = (float)::atof(line.substr(begin, line.length()-1).c_str());
        camera = glm::vec3(cameraArray[0], cameraArray[1], cameraArray[2]);
        
        //Get camera axis - p
        getline(worldDataStream, line);
        
        begin = 0;
        end = 0;
        float pArray[3];
        int pCounter = 0;
        
        for (int i = 0; i < line.length(); i++)
        {
            if (isspace(line[i]))
            {
                end = i;
                pArray[pCounter] = (float)::atof(line.substr(begin, end).c_str());
                begin = i+1;
                pCounter++;
            }
        }
        
        pArray[pCounter] = (float)::atof(line.substr(begin, line.length()-1).c_str());
        p = glm::vec3(pArray[0], pArray[1], pArray[2]);
        
        //Get camera axis - q
        getline(worldDataStream, line);
        
        begin = 0;
        end = 0;
        float qArray[3];
        int qCounter = 0;
        
        for (int i = 0; i < line.length(); i++)
        {
            if (isspace(line[i]))
            {
                end = i;
                qArray[qCounter] = (float)::atof(line.substr(begin, end).c_str());
                begin = i+1;
                qCounter++;
            }
        }
        
        qArray[qCounter] = (float)::atof(line.substr(begin, line.length()-1).c_str());
        q = glm::vec3(qArray[0], qArray[1], qArray[2]);
        
        //Get camera axis - r
        getline(worldDataStream, line);
        
        begin = 0;
        end = 0;
        float rArray[3];
        int rCounter = 0;
        
        for (int i = 0; i < line.length(); i++)
        {
            if (isspace(line[i]))
            {
                end = i;
                rArray[rCounter] = (float)::atof(line.substr(begin, end).c_str());
                begin = i+1;
                rCounter++;
            }
        }
        
        rArray[rCounter] = (float)::atof(line.substr(begin, line.length()-1).c_str());
        r = glm::vec3(rArray[0], rArray[1], rArray[2]);
        
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
                    photoNames.push_back("0");
                }
                else if (line == "1")
                {
                    movableFlags.push_back(1);
                    photoNames.push_back("0");
                }
                else if (line == "2")
                {
                    movableFlags.push_back(2);
                    getline(worldDataStream, line);
                    photoNames.push_back(line);
                }
                else if (line == "3")
                {
                    movableFlags.push_back(3);
                    photoNames.push_back("0");
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