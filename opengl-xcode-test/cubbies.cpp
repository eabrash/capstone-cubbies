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

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <FreeImage.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Self-made files from the project
#include "shaderprogram.h"
#include "worldloader.h"
#include "mesh.h"
#include "model.h"

//

void updateWallArtPosition(GLFWwindow *window, Model *focalModel, float step, float angle, glm::vec3 camera, glm::vec3 p, glm::vec3 q, glm::vec3 r, std::vector<Model*> &models, int focalModelIndex)
{
    
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
        if (i != focalModelIndex)
        {
            std::vector<glm::vec3> boundingBoxTarget = models[i]->getBoundingBox();
            std::vector<glm::vec3> boundingBoxFocal = focalModel->getBoundingBox();
            
            
            if (i == 0)
            {
                std::cout << "TARGET BOUNDING BOX: \n";
                
                for (int i = 0; i < boundingBoxTarget.size(); i++)
                {
                    std::cout << boundingBoxTarget[i].x << " " << boundingBoxTarget[i].y << " " << boundingBoxTarget[i].z << "\n";
                }
                
                std::cout << "FOCAL BOUNDING BOX: \n";
                
                for (int i = 0; i < boundingBoxFocal.size(); i++)
                {
                    std::cout << boundingBoxFocal[i].x << " " << boundingBoxFocal[i].y << " " << boundingBoxFocal[i].z << "\n";
                }
            }
            
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

int main(){
    
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
    const int WINDOW_WIDTH = 800;
    
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
    
    loadWorld("world1.txt", filenames, translations, scales, rotations, movables, splitMeshes);

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
        numMeshes += newModel->getNumMeshes();
    }
    
    // Main drawing loop
    
    glm::vec3 lightPositionWorld = glm::vec3(0.0f, 7.0f, 0.0f);
    
    float step = 0.05;
    glm::vec3 camera = glm::vec3(0.0f, 5.5f, 0.0f);
    glm::vec3 p = glm::vec3(1,0,0);       // +Y-axis of camera (basis vector) - up
    glm::vec3 q = glm::vec3(0,1,0);       // +X-axis of camera (basis vector) - right
    glm::vec3 r = glm::vec3(0,0,-1);      // -Z-axis of camera (basis vector) - front
    float angle = 0.01; // Increment to look up or down by
    
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 )
    {
        // Move forward or back
        // Projection of vector onto plane explanation: https://www.maplesoft.com/support/help/Maple/view.aspx?path=MathApps/ProjectionOfVectorOntoPlane
        
        glm::vec3 oldCamera = camera;
        glm::vec3 oldP = p;       // +Y-axis of camera (basis vector) - up
        glm::vec3 oldQ = q;       // +X-axis of camera (basis vector) - right
        glm::vec3 oldR = r;      // -Z-axis of camera (basis vector) - front
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS  || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
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
                    updateWallArtPosition(window, models[focalModel], step, angle, camera, p, q, r, models, focalModel);
                }
            }
        }
        
        for (int i = 0; i < numModels; i++)
        {
            if (models[i]->collidedWithPlayer(camera, p, q, r))
            {
                camera = oldCamera;
                p = oldP;
                q = oldQ;
                r = oldR;
                break;
            }
        }
    
        
        glm::mat4 viewMatrix = glm::lookAt(
                                           camera, // position of camera
                                           camera + r, // where to look
                                           glm::vec3(0.0f, 1.0f, 0.0f)    // +Y axis points up
                                           );
        
        //std::cout << "Camera position: " << camera.x << ", " << camera.y << ", " << camera.z << "\n";
        
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)4/3, 0.1f, 1000.0f);
        
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]); // Locn, count, transpose, value
        glUniform3f(LightPositionID, lightPositionWorld.x, lightPositionWorld.y, lightPositionWorld.z);
        glUniform3f(CameraPositionID, camera.x, camera.y, camera.z);
        
        // From opengl-tutorial.org, "Picking with an OpenGL Hack"
        // http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-opengl-hack/
        
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
        {
            glUseProgram(PickingProgramID); // Use the shader program to do the drawing
            
            glClearColor(1.0, 1.0, 1.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen to dark blue, also depth buffer
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
            
            //std::cout << "xPos: " << xPos << ", yPos: " << yPos << "\n";
            
//            double adjustedXPos = (2.0f * xPos) / WINDOW_WIDTH - 1.0f;
//            double adjustedYPos = 1.0f - (2.0f * yPos) / WINDOW_HEIGHT;
            
            double adjustedXPos = xPos*2-1;
            double adjustedYPos = (WINDOW_HEIGHT-yPos)*2-1;
            
            //std::cout << "adjustedXPos = " << adjustedXPos << ", adjustedYPos = " << adjustedYPos << "\n";
            
            glReadPixels(adjustedXPos, adjustedYPos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColorData);
            
            int modelNumber = pixelColorData[0] + pixelColorData[1]*256 + pixelColorData[2]*256*256;
            //std::cout << modelNumber << " was clicked\n";
            
            if (modelNumber != 0xFFFFFF && models[modelNumber]->isMovable())
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
                
                // 3rd attribute buffer: UVs
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
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }
    
    // Cleanup VBO and shader
    
    for (int i = 0; i < numMeshes; i++)
    {
//        glDeleteBuffers(1, &vertexbuffer[i]);
//        glDeleteBuffers(1, &uvbuffer[i]);
//        glDeleteBuffers(1, &normalbuffer[i]);
//        glDeleteBuffers(1, &indexbuffer[i]);
    }
    glDeleteProgram(ProgramID);
    glDeleteTextures(1, &TextureID);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
    
}