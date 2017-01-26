//
//  main.cpp
//
// Based on code from opengl-tutorial: http://www.opengl-tutorial.org/download/ (licensed under FTWPL).
//
// Using FreeImage (http://freeimage.sourceforge.net/license.html), GPLv3.0.
//

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <FreeImage.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <dirent.h>

// Self-made files from the project
#include "shaderprogram.h"
#include "worldloader.h"
#include "mesh.h"
#include "model.h"
#include "worldwriter.h"
#include "imageloader.h"

// Code for reading files in a directory...
// From Rafael Baptista: http://stackoverflow.com/questions/11140483/how-to-get-list-of-files-with-a-specific-extension-in-a-given-folder
// From Chris Kloberdanz: http://pubs.opengroup.org/onlinepubs/009695399/functions/readdir_r.html
// Reference for dirent.h: http://pubs.opengroup.org/onlinepubs/009695399/functions/readdir_r.html

bool loadPhotos(std::vector<std::string> &photoNames, std::vector<GLuint> &photoTextures)
{
    DIR* myDirectory; // Directory stream
    myDirectory = opendir("photos");
    
    if (myDirectory != nullptr)
    {
        struct dirent *directoryEntry; // Directory entry, i.e., file
        std::vector<std::string> photoFileNames;
        
        while ((directoryEntry = readdir(myDirectory)) != nullptr)
        {
            std::string fileName(directoryEntry->d_name);
            
            if (fileName.length() >= 4 && fileName.substr(fileName.length()-4, 4) == ".bmp")
            {
                photoNames.push_back(fileName);
                fileName = "photos/" + fileName;
                photoTextures.push_back(loadBMP(fileName.c_str()));
                std::cout << fileName << "\n";
            }
        }
        
        closedir(myDirectory);
        
        return true;
    }
    else
    {
        std::cout << "Problem finding directory" << "\n";
        return false;
    }
}

void updateWallArtPosition(GLFWwindow *window, Model *focalModel, float step, float angle, glm::vec3 camera, glm::vec3 p, glm::vec3 q, glm::vec3 r, std::vector<Model*> &models, int focalModelIndex, std::vector<GLuint> photoTextures, bool rightKeyPressedLastFrame)
{
    glm::mat4 originalTranslation = focalModel->getTranslation();
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        glm::mat4 translation = focalModel->getTranslation();
        translation[3][1] += step;
        focalModel->setTranslation(translation);
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        glm::mat4 translation = focalModel->getTranslation();
        translation[3][1] -= step;
        focalModel->setTranslation(translation);
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        // Needs to be the l and r of model, not l and r of world
        glm::mat4 objectToWorld = focalModel->getTranslation() * focalModel->getRotation() * focalModel->getScale();
        
        glm::vec3 r = glm::normalize(glm::vec3(objectToWorld * glm::vec4(0, 0, 1, 0)));
        
        glm::vec3 adjustedStep = step * r; // Why is this R and not P?
        
        glm::mat4 translation = focalModel->getTranslation();
        
        translation[3][0] += adjustedStep.x;
        translation[3][1] += adjustedStep.y;
        translation[3][2] += adjustedStep.z;
        
        focalModel->setTranslation(translation);
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        glm::mat4 objectToWorld = focalModel->getTranslation() * focalModel->getRotation() * focalModel->getScale();
        
        glm::vec3 r = glm::normalize(glm::vec3(objectToWorld * glm::vec4(0, 0, 1, 0)));
        
        glm::vec3 adjustedStep = step * r;
        
        glm::mat4 translation = focalModel->getTranslation();
        
        translation[3][0] -= adjustedStep.x;
        translation[3][1] -= adjustedStep.y;
        translation[3][2] -= adjustedStep.z;
        
        focalModel->setTranslation(translation);
    }
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && !rightKeyPressedLastFrame)
    {
        std::cout << "CHANGING THE PICTURE\n";
        
        std::vector<GLuint> textures = focalModel->getTextures();
        
        int next = -1;
        
        for (int i = 0; i < photoTextures.size(); i++)
        {
            if (textures[1] == photoTextures[i])
            {
                next = (i+1) % photoTextures.size();
                textures[1] = photoTextures[next]; // Only giving black screen
                break;
            }
        }
        
        if (next == -1)
        {
            textures[1] = photoTextures[0];
        }
        
        focalModel->setTextures(textures);
    }
    
    // Add handling of collision with camera box also.
    // Image can go off edge of wall (nothing to stop it at the moment). Needs to check if there is scenery
    // underneath it.
    
    for (int i = 0; i < models.size(); i++)
    {
        if (i != focalModelIndex && models[i]->isMovable() != 3)
        {
            if (focalModel->collidedWithObject(models[i]))
            {
                focalModel->setTranslation(originalTranslation);
            }
        }
    }
}

// Move a selected floor-resting object F/B or R/L, or rotate it

void updateObjectPosition(GLFWwindow *window, Model *focalModel, float step, float angle, glm::vec3 camera, glm::vec3 p, glm::vec3 q, glm::vec3 r, std::vector<Model*> &models, int focalModelIndex)
{
    glm::mat4 originalTranslation = focalModel->getTranslation();
    glm::mat4 originalRotation = focalModel->getRotation();
    glm::mat4 originalScale = focalModel->getScale();
    
    // Move object forward/backward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        glm::mat4 translation = focalModel->getTranslation();
        
        //glm::vec3 position = translation * rotation * scale * glm::vec4(0,0,0,1);
        
        glm::vec3 inCameraDirection = glm::mat3(p,q,r)*glm::vec3(0,0,1);
        
        float hypotenuse = glm::sqrt(inCameraDirection.x * inCameraDirection.x + inCameraDirection.z * inCameraDirection.z);

        float zStep = step * inCameraDirection.z/hypotenuse;
        float xStep = step * inCameraDirection.x/hypotenuse;
        
        translation[3][0] = translation[3][0] + xStep;
        translation[3][2] = translation[3][2] + zStep;
        
        focalModel->setTranslation(translation);
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        glm::mat4 translation = focalModel->getTranslation();
        
        glm::vec3 inCameraDirection = glm::mat3(p,q,r)*glm::vec3(0,0,1);
        
        float hypotenuse = glm::sqrt(inCameraDirection.x * inCameraDirection.x + inCameraDirection.z * inCameraDirection.z);
        
        float zStep = step * inCameraDirection.z/hypotenuse;
        float xStep = step * inCameraDirection.x/hypotenuse;

        translation[3][0] = translation[3][0] - xStep;
        translation[3][2] = translation[3][2] - zStep;
        
        focalModel->setTranslation(translation);
    }
    
    //Strafe sideways
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        glm::mat4 translation = focalModel->getTranslation();
        
        glm::vec3 inCameraDirection = glm::mat3(p,q,r)*glm::vec3(0,0,1);
        
        float hypotenuse = glm::sqrt(inCameraDirection.x * inCameraDirection.x + inCameraDirection.z * inCameraDirection.z);
        
        float zStep = step * inCameraDirection.z/hypotenuse;
        float xStep = step * inCameraDirection.x/hypotenuse;
        
        translation[3][0] = translation[3][0] + zStep;
        translation[3][2] = translation[3][2] - xStep;
        
        focalModel->setTranslation(translation);
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        glm::mat4 translation = focalModel->getTranslation();
        
        glm::vec3 inCameraDirection = glm::mat3(p,q,r)*glm::vec3(0,0,1);
        
        float hypotenuse = glm::sqrt(inCameraDirection.x * inCameraDirection.x + inCameraDirection.z * inCameraDirection.z);
        
        float zStep = step * inCameraDirection.z/hypotenuse;
        float xStep = step * inCameraDirection.x/hypotenuse;
        
        translation[3][0] = translation[3][0] - zStep;
        translation[3][2] = translation[3][2] + xStep;

        focalModel->setTranslation(translation);
    }
    
    //Rotate in X-Z plane
    
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        glm::mat4 rotation = glm::mat4();
        
        rotation[0] = glm::vec4(cos(-angle), 0, -sin(-angle), 0);
        rotation[1] = glm::vec4(0, 1, 0, 0);
        rotation[2] = glm::vec4(sin(-angle), 0, cos(-angle), 0);
        
        focalModel->setRotation(rotation * focalModel->getRotation());
    }
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        glm::mat4 rotation = glm::mat4();
        
        rotation[0] = glm::vec4(cos(angle), 0, -sin(angle), 0);
        rotation[1] = glm::vec4(0, 1, 0, 0);
        rotation[2] = glm::vec4(sin(angle), 0, cos(angle), 0);
        
        focalModel->setRotation(rotation * focalModel->getRotation());
    }
    
    // Check if the movement caused the object to collide with either the camera or the other objects
    // in the scene - reset its position if so
    
    // With the camera
    
    if (focalModel->collidedWithPlayer(camera, p, q, r))
    {
        focalModel->setTranslation(originalTranslation);
        focalModel->setRotation(originalRotation);
        focalModel->setScale(originalScale);
    }

    // With other objects
    
    for (int i = 0; i < models.size(); i++)
    {
        if (i != focalModelIndex && models[i]->isMovable() != 3)
        {
            std::vector<glm::vec3> boundingBoxTarget = models[i]->getBoundingBox();
            std::vector<glm::vec3> boundingBoxFocal = focalModel->getBoundingBox();
            
            if (models[i]->collidedWithObject(focalModel))
            {
                std::cout << "COLLISION DETECTED: model " << focalModelIndex << " with model " << i << "\n";

                focalModel->setTranslation(originalTranslation);
                focalModel->setRotation(originalRotation);
                focalModel->setScale(originalScale);
                break;
            }
        }
    }
}

// Adjust position or orientation of camera in response to keypress

void updateCameraPosition(GLFWwindow *window, glm::vec3 &camera, glm::vec3 &p, glm::vec3 &q, glm::vec3 &r, float step, float angle)
{
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        glm::vec3 inCameraDirection = glm::mat3(p,q,r)*glm::vec3(0,0,1);
        //std::cout << "inCameraDirection: " << inCameraDirection.x << inCameraDirection.y <<inCameraDirection.z << "\n";
        camera = camera + glm::normalize(glm::vec3(inCameraDirection.x, 0, inCameraDirection.z))*step;
        
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        glm::vec3 inCameraDirection = glm::mat3(p,q,r)*glm::vec3(0,0,1);
        //std::cout << "inCameraDirection: " << inCameraDirection.x << inCameraDirection.y <<inCameraDirection.z << "\n";
        camera = camera + glm::normalize(glm::vec3(inCameraDirection.x, 0, inCameraDirection.z))*(-step);
    }
    
    // Strafe sideways
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera = camera + glm::mat3(p,q,r)*glm::vec3(-step,0,0);
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera = camera + glm::mat3(p,q,r)*glm::vec3(step,0,0);
    }
    
    glm::mat3 rotation = glm::mat3();
    
    //Turn viewer's gaze up or down
    
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        // Rotate about x-axis; don't let the viewer look beyond 90 degrees down (stop slightly short)
        
        if (glm::dot(glm::vec3(0, 1, 0), r) > -0.95)
        {
            // This approach ensures that we specifically rotate about the model's own x-axis
            // p (expressed in world coordinates), not around the x-axis of the world
            
            glm::mat4 rotationMatrix = glm::rotate(-angle, p);
            
            glm::vec4 rotatedQ = rotationMatrix*glm::vec4(q,0);
            glm::vec4 rotatedR = rotationMatrix*glm::vec4(r,0);
            
            q = glm::vec3(rotatedQ.x, rotatedQ.y, rotatedQ.z);
            r = glm::vec3(rotatedR.x, rotatedR.y, rotatedR.z);
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        // Rotate about x-axis; don't let the viewer look beyond 90 degrees up (stop slightly short)
        
        if (glm::dot(glm::vec3(0, 1, 0), r) < 0.95)
        {
            glm::mat4 rotationMatrix = glm::rotate(angle, p);
            
            glm::vec4 rotatedQ = rotationMatrix*glm::vec4(q,0);
            glm::vec4 rotatedR = rotationMatrix*glm::vec4(r,0);
            
            q = glm::vec3(rotatedQ.x, rotatedQ.y, rotatedQ.z);
            r = glm::vec3(rotatedR.x, rotatedR.y, rotatedR.z);
        }
    }
    
    // Turn viewer's gaze and direction of motion to the side
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        //rotate about y-axis
        rotation[0] = glm::vec3(cos(-angle), 0, -sin(-angle));
        rotation[1] = glm::vec3(0, 1, 0);
        rotation[2] = glm::vec3(sin(-angle), 0, cos(-angle));
        
        p = glm::normalize(rotation*p);
        r = glm::normalize(rotation*r);
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        //rotate about y-axis
        rotation[0] = glm::vec3(cos(angle), 0, -sin(angle));
        rotation[1] = glm::vec3(0, 1, 0);
        rotation[2] = glm::vec3(sin(angle), 0, cos(angle));
        
        p = glm::normalize(rotation*p);
        r = glm::normalize(rotation*r);
    }
}

bool placeDuplicateModel(Model *duplicateModel, int focalModel, std::vector<Model *> &models, int &numModels, glm::vec3 camera, glm::vec3 p, glm::vec3 q, glm::vec3 r)
{
    bool foundValidPlacement = true;
    
    // Determine the model's dimensions in x and z directions
    glm::vec3 scaledMaxes = duplicateModel->getScale() * glm::vec4(duplicateModel->getMaxesObjectSpace(), 1);
    glm::vec3 scaledMins = duplicateModel->getScale() * glm::vec4(duplicateModel->getMinsObjectSpace(), 1);
    float xWidth = scaledMaxes.x - scaledMins.x + 0.01;
    float zWidth = scaledMaxes.z - scaledMins.z + 0.01;
    
    // Move the new model along + z axis relative to old model
    if (true)
    {
        glm::mat4 newTranslation = duplicateModel->getRotation()*glm::translate(glm::vec3(0, 0, zWidth));
        glm::mat4 translation = models[focalModel]->getTranslation();
        
        float targetX = translation[3].x + newTranslation[3].x;
        float targetZ = translation[3].z + newTranslation[3].z;
        
        std::cout << "targetX = " << targetX << "\n";
        std::cout << "targetZ = " << targetZ << "\n";
        
        std::cout << "current X= " << translation[3].x << "\n";
        std::cout << "current Z = " << translation[3].z << "\n";
        
        
        while (glm::abs(translation[3].x-targetX) >= 0.001 || glm::abs(translation[3].z-targetZ)>=0.001)
        {
            translation[3].x += 0.01 * newTranslation[3].x;
            translation[3].z += 0.01 * newTranslation[3].z;
            duplicateModel->setTranslation(translation);
            
            for (int i = 0; i < numModels; i++)
            {
                if (i != focalModel && models[i]->isMovable() != 3 && duplicateModel->collidedWithObject(models[i]))
                {
                    foundValidPlacement = false;
                    break;
                }
            }
            
            if (foundValidPlacement && duplicateModel->collidedWithPlayer(camera, p, q, r))
            {
                foundValidPlacement = false;
            }
        }
    }
    
    std::cout << "After moving to +zWidth: " << foundValidPlacement << "\n";
    
    // Move the new model along -z axis relative to old model
    if (!foundValidPlacement)
    {
        foundValidPlacement = true;
        
        glm::mat4 newTranslation = duplicateModel->getRotation()*glm::translate(glm::vec3(0, 0, -zWidth));
        glm::mat4 translation = models[focalModel]->getTranslation();
        
        float targetX = translation[3].x + newTranslation[3].x;
        float targetZ = translation[3].z + newTranslation[3].z;
        
        std::cout << "targetX = " << targetX << "\n";
        std::cout << "targetZ = " << targetZ << "\n";
        
        std::cout << "current X= " << translation[3].x << "\n";
        std::cout << "current Z = " << translation[3].z << "\n";
        
        
        while (glm::abs(translation[3].x-targetX) >= 0.001 || glm::abs(translation[3].z-targetZ)>=0.001)
        {
            translation[3].x += 0.01 * newTranslation[3].x;
            translation[3].z += 0.01 * newTranslation[3].z;
            duplicateModel->setTranslation(translation);
            
            for (int i = 0; i < numModels; i++)
            {
                if (i != focalModel && models[i]->isMovable() != 3 && duplicateModel->collidedWithObject(models[i]))
                {
                    foundValidPlacement = false;
                    std::cout << "INVALID B/C I CRUNCHED INTO MODEL " << i << "\n";
                    break;
                }
            }
            
            if (foundValidPlacement && duplicateModel->collidedWithPlayer(camera, p, q, r))
            {
                std::cout << "INVALID B/C I CRUNCHED INTO THE PLAYER " << "\n";
                foundValidPlacement = false;
            }
        }
    }
    
    std::cout << "After moving to -zWidth: " << foundValidPlacement << "\n";
    
    // Move the new model along +x axis relative to old model
    if (!foundValidPlacement)
    {
        foundValidPlacement = true;
        
        glm::mat4 newTranslation = duplicateModel->getRotation()*glm::translate(glm::vec3(xWidth, 0, 0));
        glm::mat4 translation = models[focalModel]->getTranslation();
        
        float targetX = translation[3].x + newTranslation[3].x;
        float targetZ = translation[3].z + newTranslation[3].z;
        
        std::cout << "targetX = " << targetX << "\n";
        std::cout << "targetZ = " << targetZ << "\n";
        
        std::cout << "current X= " << translation[3].x << "\n";
        std::cout << "current Z = " << translation[3].z << "\n";
        
        
        while (glm::abs(translation[3].x-targetX) >= 0.001 || glm::abs(translation[3].z-targetZ)>=0.001)
        {
            translation[3].x += 0.01 * newTranslation[3].x;
            translation[3].z += 0.01 * newTranslation[3].z;
            duplicateModel->setTranslation(translation);
            
            for (int i = 0; i < numModels; i++)
            {
                if (i != focalModel && models[i]->isMovable() != 3 && duplicateModel->collidedWithObject(models[i]))
                {
                    foundValidPlacement = false;
                    break;
                }
            }
            
            if (foundValidPlacement && duplicateModel->collidedWithPlayer(camera, p, q, r))
            {
                foundValidPlacement = false;
            }
        }
    }
    
    std::cout << "After moving to +xWidth: " << foundValidPlacement << "\n";
    
    // Move the new model along -x axis relative to old model
    if (!foundValidPlacement)
    {
        foundValidPlacement = true;
        
        glm::mat4 newTranslation = duplicateModel->getRotation()*glm::translate(glm::vec3(-xWidth, 0, 0));
        glm::mat4 translation = models[focalModel]->getTranslation();
        
        float targetX = translation[3].x + newTranslation[3].x;
        float targetZ = translation[3].z + newTranslation[3].z;
        
        //                std::cout << "targetX = " << targetX << "\n";
        //                std::cout << "targetZ = " << targetZ << "\n";
        //
        //                std::cout << "current X= " << translation[3].x << "\n";
        //                std::cout << "current Z = " << translation[3].z << "\n";
        
        
        while (glm::abs(translation[3].x-targetX) >= 0.001 || glm::abs(translation[3].z-targetZ)>=0.001)
        {
            translation[3].x += 0.01 * newTranslation[3].x;
            translation[3].z += 0.01 * newTranslation[3].z;
            duplicateModel->setTranslation(translation);
            
            for (int i = 0; i < numModels; i++)
            {
                if (i != focalModel && models[i]->isMovable() != 3 && duplicateModel->collidedWithObject(models[i]))
                {
                    foundValidPlacement = false;
                    break;
                }
            }
            
            if (foundValidPlacement && duplicateModel->collidedWithPlayer(camera, p, q, r))
            {
                foundValidPlacement = false;
            }
        }
    }
    
    std::cout << "After moving to -xWidth: " << foundValidPlacement << "\n";
    
    if (foundValidPlacement)
    {
        models.push_back(duplicateModel);
        numModels += 1;
        focalModel = numModels-1;
    }
    
    return foundValidPlacement;
}

int main(int argc, const char * argv[]){
    
    // Code snippet from: http://stackoverflow.com/questions/40920783/xcode-app-no-longer-reads-input-from-the-folder-the-app-is-stored-in
    
    char workingDir[1024];
    
    getcwd(workingDir, 1024);
    
    std::cout << workingDir << "\n";
    
    // argv[0] returns the full path to the program
    std::string directory(argv[0]);
    // And we want to get rid of the program name
    directory = directory.substr(0, directory.find_last_of("/"));
    // Point the directory to the program directory
    chdir(directory.c_str());
    
    std::cout << "DIRECTORY I AM IN: " << directory.c_str() << "\n";
    
    // GLFW setup
    
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing. What is antialiasing?
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3; this line is the ones place 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // This line is the tenths place 3
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    const int WINDOW_HEIGHT = 600;
    const int WINDOW_WIDTH = 1200;
    
    GLFWwindow * window = nullptr;
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cubbies", NULL, NULL);
    if (window == nullptr)
    {
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // The window is our current rendering context
    
    // GLEW setup
    glewExperimental = true; // The core profile needs for this flag to be set
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); // Input mode on window is to detect keystrokes
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    
    // Turn on culling; cull triangles with their back facing the camera
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Turn on z-buffering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);   // Keep the fragment w/shorter distance to camera
    
    // Make the VAO
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);     // This is the VAO
    
    // Create and compile our GLSL program from the shaders
    GLuint ProgramID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
    GLuint PickingProgramID = LoadShaders("PickingVertexShader.txt", "PickingFragmentShader.txt");
    
    // This section is getting handles for uniforms in the shader
    GLuint MatrixID = glGetUniformLocation(ProgramID, "MY_MATRIX");
    GLuint ViewMatrixID = glGetUniformLocation(ProgramID, "VIEW_MATRIX");
    GLuint ModelMatrixID = glGetUniformLocation(ProgramID, "MODEL_MATRIX");
    GLuint LightPositionID = glGetUniformLocation(ProgramID, "LIGHT_POSITION_WORLDSPACE");
    GLuint CameraPositionID = glGetUniformLocation(ProgramID, "CAMERA_POSITION_WORLDSPACE");
    GLuint TextureID = glGetUniformLocation(ProgramID, "myTextureSampler");
    GLuint FocalID = glGetUniformLocation(ProgramID, "IN_FOCUS");
    GLuint BackgroundStateID = glGetUniformLocation(ProgramID, "IS_BACKGROUND");
    GLuint NumLightsID = glGetUniformLocation(ProgramID, "NUM_LIGHTS");
    //GLuint ModelMatrixInverseTransposeID = glGetUniformLocation(ProgramID, "MODEL_MATRIX_INVERSE_TRANSPOSE");
    
    GLuint PickingMatrixID = glGetUniformLocation(PickingProgramID, "MY_PICKING_MATRIX");
    GLuint PickingColorID = glGetUniformLocation(PickingProgramID, "MY_PICKING_COLOR");
    
    // Load the file that tells us what models we will have in the world and what their initial
    // transformations will be.
    
    std::vector<std::string> filenames;
    std::vector<glm::mat4> translations;
    std::vector<glm::mat4> scales;
    std::vector<glm::mat4> rotations;
    std::vector<int> movables;
    std::vector<bool> splitMeshes;
    std::vector<std::string> photoNames;
    
    std::vector<glm::vec3> lightPositionsWorld;
    std::vector<float> lightPowers;
    glm::vec3 camera;
    glm::vec3 p;
    glm::vec3 q;
    glm::vec3 r;
    
    loadWorld("output.txt", filenames, translations, scales, rotations, movables, splitMeshes, lightPositionsWorld, lightPowers, camera, p, q, r, photoNames);

    std::vector<GLuint> photoTextures;
    std::vector<std::string> photoFilenames;
    
    loadPhotos(photoFilenames, photoTextures);
    
    int numModels = filenames.size();
    
    int focalModel = -1; // Negative number means that no model is selected
    
    // Read in each .obj file to create a model. Mesh objects from each .obj file are stored inside the
    // model object in a std::vector.
    
    int numMeshes = 0;
    
    std::vector<Model*> models;
    for (int i = 0; i < numModels; i++)
    {
        Model *newModel = new Model(filenames[i], translations[i], scales[i], rotations[i], movables[i],splitMeshes[i]);
        models.push_back(newModel);
        
        // Store the pictures loaded in picture frames in the photo library
        
        if (newModel->isMovable() == 2)
        {
            std::vector<GLuint> textures = newModel->getTextures();
            
            for (int j = 0; j < photoFilenames.size(); j++)
            {
                if (photoNames[i] == photoFilenames[j])
                {
                    textures[1] = photoTextures[j];
                    newModel->setTextures(textures);
                }
            }
        }
        
        numMeshes += newModel->getNumMeshes();
    }
    
    
    
//    int i = 2;
//    
//    Model *newModel = new Model(filenames[i], translations[i], scales[i], rotations[i], movables[i],splitMeshes[i]);
//    newModel->setTranslation(newModel->getTranslation() + glm::translate(glm::vec3(0,3,0)));
//    models.push_back(newModel);
//    numModels += 1;
    
    // Main drawing loop
    
    float step = 0.05;
    float angle = 0.01; // Increment to look up or down by
    
    int numLoopIterations = 0;
    bool rightKeyPressedLastFrame = false;
    bool plusKeyPressedLastFrame = false;
    bool minusKeyPressedLastFrame = false;
    
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 )
    {
        // Move forward or back
        // Projection of vector onto plane explanation: https://www.maplesoft.com/support/help/Maple/view.aspx?path=MathApps/ProjectionOfVectorOntoPlane
        
        glm::vec3 oldCamera = camera;
        glm::vec3 oldP = p;       // +Y-axis of camera (basis vector) - up
        glm::vec3 oldQ = q;       // +X-axis of camera (basis vector) - right
        glm::vec3 oldR = r;      // -Z-axis of camera (basis vector) - front
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS  || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            if (focalModel < 0)
            {
                updateCameraPosition(window, camera, p, q, r, step, angle);  // Camera is moved
            }
            else
            {
                if (models[focalModel]->isMovable() == 1)   // Object rests on floor
                {
                    updateObjectPosition(window, models[focalModel], step, angle, camera, p, q, r, models, focalModel);
                }
                else if (models[focalModel]->isMovable() == 2) // Object is posted vertically on wall
                {
                    updateWallArtPosition(window, models[focalModel], step, angle, camera, p, q, r, models, focalModel, photoTextures, rightKeyPressedLastFrame);
                }
            }
            
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                rightKeyPressedLastFrame = true;
            }
            else
            {
                rightKeyPressedLastFrame = false;
            }
        }
        
        for (int i = 0; i < numModels; i++)
        {
            // If isMovable() == 3, the object in question is the background sphere and should not be
            // considered - the world should be laid out so the player cannot approach the background
            
            if (models[i]->isMovable() != 3 && models[i]->collidedWithPlayer(camera, p, q, r))
            {
                camera = oldCamera;
                p = oldP;
                q = oldQ;
                r = oldR;
                break;
            }
        }
        
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
        {
            if (!plusKeyPressedLastFrame && focalModel != -1 && models[focalModel]->isMovable() == 1)
            {
                // Make a (shallow) copy of the selected model
                Model *duplicateModel = new Model(*models[focalModel]);
                placeDuplicateModel(duplicateModel, focalModel, models, numModels, camera, p, q, r);
            }
        
            if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
            {
                plusKeyPressedLastFrame = true;
            }
            else
            {
                plusKeyPressedLastFrame = false;
            }
        }
    
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
        {
            if (!minusKeyPressedLastFrame && models[focalModel]->isMovable() == 1)
            {
                models.erase(models.begin() + focalModel);
                numModels -= 1;
                focalModel = -1;
            }
            
            if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
            {
                minusKeyPressedLastFrame = true;
            }
            else
            {
                minusKeyPressedLastFrame = false;
            }
        }

        
        glm::mat4 viewMatrix = glm::lookAt(
                                           camera, // position of camera
                                           camera + r, // where to look
                                           glm::vec3(0.0f, 1.0f, 0.0f)    // +Y axis points up
                                           );
        
        //std::cout << "Camera position: " << camera.x << ", " << camera.y << ", " << camera.z << "\n";
        
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)1200/600, 0.1f, 200000.0f);
        
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]); // Locn, count, transpose, value
        glUniform3f(CameraPositionID, camera.x, camera.y, camera.z);
        
        // How to pass in an array of light positions (vec3s) from:
        // https://www.opengl.org/discussion_boards/showthread.php/198200-Passing-array-of-vec3-to-fragment-shader
        
        glUniform3fv(LightPositionID, lightPositionsWorld.size(), &lightPositionsWorld[0].x);
        glUniform1i(NumLightsID, lightPositionsWorld.size());
        
        // From opengl-tutorial.org, "Picking with an OpenGL Hack"
        // http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-opengl-hack/
        
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
        {
            glUseProgram(PickingProgramID); // Use the shader program to do the drawing
            
            glClearColor(1.0, 1.0, 1.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen, also depth buffer
            glEnable(GL_DEPTH_TEST);
            
            for (int i = 0; i < numModels; i++)
            {
                // Convert "i", the integer mesh ID, into an RGB color
                int r = (i & 0x000000FF) >>  0; // Bit mask: take least sig 2 bits and make be R
                int g = (i & 0x0000FF00) >>  8; // Bit mask: take next least sig 2 bits and make be R
                int b = (i & 0x00FF0000) >> 16; // Bit mask: take second most sig 2 bits and make be R
                
                //std::cout << "r: " << r << ", g: " << g << ", b: " << b << "\n";
                
                std::vector<Mesh> *pMeshes = models[i]->getMeshes();

                glm::mat4 myModelMatrix = models[i]->getTranslation() * models[i]->getRotation() * models[i]->getScale();
                glm::mat4 mymatrix = projectionMatrix * viewMatrix * myModelMatrix;
                
                glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &mymatrix[0][0]); // Sending matrix to the shader
                glUniform4f(PickingColorID, r/255.0f, g/255.0f, b/255.0f, 1.0f);
                
                for (int j = 0; j < models[i]->getNumMeshes(); j++)
                {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*pMeshes)[j].getIndexBuffer());
                    
                    // 1st attribute buffer: locations of vertices
                    glEnableVertexAttribArray(0);
                    glBindBuffer(GL_ARRAY_BUFFER, (*pMeshes)[j].getVertexBuffer());
                    glVertexAttribPointer(
                                          0,
                                          3,                                // size
                                          GL_FLOAT,                         // type
                                          GL_FALSE,                         // normalized?
                                          0,                                // stride
                                          (void*)0                          // array buffer offset
                                          );
                    
                    glDrawElements(GL_TRIANGLES, (*pMeshes)[j].getNumFaces() * 3, GL_UNSIGNED_SHORT, (void*)0);
                    //glDrawArrays(GL_TRIANGLES, 0, vertices.size());
                    
                    glDisableVertexAttribArray(0);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                }
            }
            
            glFlush();
            glFinish();
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Something about how glReadPixels deals with "memory alignment:
            
            unsigned char pixelColorData[4];
            
            double xPos = 0;
            double yPos = 0;
            
            glfwGetCursorPos(window, &xPos, &yPos);
            
            int width = 0;
            int height = 0;
            
            glfwGetWindowSize(window, &width, &height);
            
            int fWidth = 0;
            int fHeight =0;
            
            glfwGetFramebufferSize(window, &fWidth, &fHeight);
            
            //            std::cout << "xPos: " << xPos << ", yPos: " << yPos << "\n";
            //            std::cout << "width - window: " << width << ", height - window: " << height <<"\n";
            //            std::cout << "width - framebuffer: " << fWidth << ", height - framebuffer: " << fHeight <<"\n";
            
            //            double adjustedXPos = (2.0f * xPos) / WINDOW_WIDTH - 1.0f;
            //            double adjustedYPos = 1.0f - (2.0f * yPos) / WINDOW_HEIGHT;
            
            double adjustedXPos = xPos*(fWidth/width)-1;
            double adjustedYPos = (WINDOW_HEIGHT-yPos)*(fWidth/width)-1;
            
            //std::cout << "adjustedXPos = " << adjustedXPos << ", adjustedYPos = " << adjustedYPos << "\n";
            
            glReadPixels(adjustedXPos, adjustedYPos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColorData);
            
            int modelNumber = pixelColorData[0] + pixelColorData[1]*256 + pixelColorData[2]*256*256;
            //std::cout << modelNumber << " was clicked\n";
            
            if (modelNumber != 0xFFFFFF && models[modelNumber]->isMovable() != 0 && models[modelNumber]->isMovable() != 3)
            {
                focalModel = modelNumber; // Arrows now move selected model
            }
            else
            {
                focalModel = -1; // Clicked off - arrows now move player
            }
            
//            glfwSwapBuffers(window);
//            glfwPollEvents();
        }
        
        // Dark blue background
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
        // Re-clear the screen for real rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        
        glUseProgram(ProgramID); // Use the shader program to do the drawing
        
        for (int i = 0; i < numModels; i++)
        {
            std::vector<Mesh> *pMeshes = models[i]->getMeshes();
            std::vector<GLuint> textures = models[i]->getTextures();
            
            glm::mat4 myModelMatrix = models[i]->getTranslation() * models[i]->getRotation() * models[i]->getScale();
            glm::mat4 mymatrix = projectionMatrix * viewMatrix * myModelMatrix;
            
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mymatrix[0][0]); // Sending matrix to the shader
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &myModelMatrix[0][0]);
            
            if (i == focalModel)
            {
                glUniform1i(FocalID, 1);
            }
            else
            {
                glUniform1i(FocalID, 0);
            }
            
            if (models[i]->isMovable() == 3)
            {
                glUniform1i(BackgroundStateID, 1);
            }
            else
            {
                 glUniform1i(BackgroundStateID, 0);
            }
            
            for (int j = 0; j < models[i]->getNumMeshes(); j++)
            {
                // Bind our texture in Texture Unit 0
                glActiveTexture(GL_TEXTURE0);
                int textureIndex = (*pMeshes)[j].getTextureIndex();
                glBindTexture(GL_TEXTURE_2D, textures[textureIndex]);
            
                // Set our "myTextureSampler" sampler to user Texture Unit 0
                glUniform1i(TextureID, 0);
                
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*pMeshes)[j].getIndexBuffer());
                
                // 1st attribute buffer: locations of vertices
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, (*pMeshes)[j].getVertexBuffer());
                glVertexAttribPointer(
                                      0,
                                      3,                                // size
                                      GL_FLOAT,                         // type
                                      GL_FALSE,                         // normalized?
                                      0,                                // stride
                                      (void*)0                          // array buffer offset
                                      );
                
                // 2nd attribute buffer: UVs
                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, (*pMeshes)[j].getUVBuffer());
                glVertexAttribPointer(
                                      1,                                // attribute
                                      2,                                // size : U+V => 2
                                      GL_FLOAT,                         // type
                                      GL_FALSE,                         // normalized?
                                      0,                                // stride
                                      (void*)0                          // array buffer offset
                                      );
                
                // 3rd attribute buffer: normals
                glEnableVertexAttribArray(2);
                glBindBuffer(GL_ARRAY_BUFFER, (*pMeshes)[j].getNormalBuffer());
                glVertexAttribPointer(
                                      2,                                // attribute
                                      3,                                // size
                                      GL_FLOAT,                         // type
                                      GL_FALSE,                         // normalized?
                                      0,                                // stride
                                      (void*)0                          // array buffer offset
                                      );
                glDrawElements(GL_TRIANGLES, (*pMeshes)[j].getNumFaces() * 3, GL_UNSIGNED_SHORT, (void*)0);
                //glDrawArrays(GL_TRIANGLES, 0, vertices.size());
                
                glDisableVertexAttribArray(0);
                glDisableVertexAttribArray(1);
                glDisableVertexAttribArray(2);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
        }
        
        // Why is this necessary to prevent a flash of heavily shadowed image on startup?
        if (numLoopIterations > 0)
        {
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        else
        {
            numLoopIterations++;
        }
    }
    
    // Cleanup VBO and shader - NEEDS TO BE FIXED
    
    for (int i = 0; i < numModels; i++)
    {
        std::vector<Mesh> *pMeshes = models[i]->getMeshes();
        
        for (int j = 0; j < numMeshes; j++)
        {
//            glDeleteBuffers(1, vertexbuffer[i]);
//            glDeleteBuffers(1, &uvbuffer[i]);
//            glDeleteBuffers(1, &normalbuffer[i]);
//            glDeleteBuffers(1, &indexbuffer[i]);
        }
    }
    
    glDeleteProgram(ProgramID);
    glDeleteTextures(1, &TextureID);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    writeWorld(models, lightPositionsWorld, lightPowers, camera, p, q, r, photoFilenames, photoTextures);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
    
}