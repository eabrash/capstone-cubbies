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
#include <glm/gtx/quaternion.hpp>

#include "worldwriter.h"
#include "model.h"

void writeWorld(std::vector<Model *> models, std::vector<glm::vec3> lightPositionsWorld, std::vector<float> lightPowers, glm::vec3 camera, glm::vec3 p, glm::vec3 q, glm::vec3 r, std::vector<std::string> photoFilenames, std::vector<GLuint> photoTextures)
{
    std::ofstream worldDataStream("output.txt", std::ios::out); // Stream from file
    
    worldDataStream << lightPositionsWorld.size() << "\n";
    
    for (int i = 0; i < lightPositionsWorld.size(); i++)
    {
        worldDataStream << lightPositionsWorld[i].x << " " << lightPositionsWorld[i].y << " " << lightPositionsWorld[i].z << "\n";
        worldDataStream << lightPowers[i] << "\n";
    }
    
    worldDataStream << camera.x << " " << camera.y << " " << camera.z << "\n";
    worldDataStream << p.x << " " << p.y << " " << p.z << "\n";
    worldDataStream << q.x << " " << q.y << " " << q.z << "\n";
    worldDataStream << r.x << " " << r.y << " " << r.z << "\n";
    
    for (int i = 0; i < models.size(); i++)
    {
        worldDataStream << models[i]->getModelFile() << "\n";
        
        glm::mat4 translation = models[i]->getTranslation();
        worldDataStream << translation[3][0] << " " << translation[3][1] << " " << translation[3][2] << "\n";
        glm::mat4 scale = models[i]->getScale();
        worldDataStream << scale[0][0] << " " << scale[1][1] << " " << scale[2][2] << "\n";
        glm::mat4 rotation = models[i]->getRotation();
        
        glm::quat rotationQuat = glm::quat_cast(rotation);
        
        float myAngle = glm::degrees(glm::angle(rotationQuat));
        glm::vec3 myAxis = glm::axis(rotationQuat);
        
        worldDataStream << myAngle << " " << myAxis.x << " " << myAxis.y << " " << myAxis.z << "\n";
        
        worldDataStream << models[i]->isMovable() << "\n";
        
        if (models[i]->isMovable() == 2)    // Framed photo
        {
            bool foundPhoto = false;
            
            for (int j = 0; j < photoTextures.size(); j++)
            {
                if (photoTextures[j] == models[i]->getTextures()[1])
                {
                    worldDataStream << photoFilenames[j] << "\n";
                    foundPhoto = true;
                }
            }
            
            if (!foundPhoto)
            {
                worldDataStream << "0" << "\n";
            }
        }
        
        if (models[i]->modelMeshesSplit())
        {
            worldDataStream << 1 << "\n";
        }
        else
        {
            worldDataStream << 0 << "\n";
        }
    }
    worldDataStream.close();
}
