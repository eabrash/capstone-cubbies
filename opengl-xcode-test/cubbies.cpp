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

// Other self-made files from the project
#include "shaderprogram.h"
#include "worldloader.h"
#include "imageloader.h"
#include "modelloader_assimp.h"

// Not yet tested. Must figure out where to calculate the bounding boxes using this function.

void getAABB(glm::mat4 &modelMatrix, std::vector<glm::vec3> &verticesOfModel, glm::vec3 &minBound, glm::vec3 &maxBound)
{
    glm::vec3 mins = verticesOfModel[0];
    glm::vec3 maxes = verticesOfModel[0];
    
    for (int i = 0; i < verticesOfModel.size(); i++)
    {
        glm::vec3 vertexInWorld = (modelMatrix * glm::vec4(verticesOfModel[i],0));
        
        if (vertexInWorld.x > maxes.x)
        {
            maxes.x = vertexInWorld.x;
        }
        
        if (vertexInWorld.y > maxes.y)
        {
            maxes.y = vertexInWorld.y;
        }
        
        if (vertexInWorld.z > maxes.z)
        {
            maxes.z = vertexInWorld.z;
        }
        
        if (vertexInWorld.x < mins.x)
        {
            mins.x = vertexInWorld.x;
        }
        
        if (vertexInWorld.y < mins.y)
        {
            mins.y = vertexInWorld.y;
        }
        
        if (vertexInWorld.z < mins.z)
        {
            mins.z = vertexInWorld.z;
        }
    }
    
    minBound = mins;
    maxBound = maxes;
}

// Main function for drawing

void drawWorldOnLoop(GLFWwindow *window, GLuint programID, GLuint ViewMatrixID, GLuint LightPositionID, GLuint CameraPositionID, int numMeshes, int *cumulativeMeshes, std::vector<GLuint> &textures, std::vector<unsigned short> &textureIndices, unsigned short *cumulativeTextures, std::vector<glm::mat4> &translations, std::vector<glm::mat4> &rotations, std::vector<glm::mat4> &scales, GLuint MatrixID, GLuint ModelMatrixID, GLuint textureID, GLuint *vertexbuffer, GLuint *normalbuffer, GLuint *uvbuffer, GLuint *indexbuffer, std::vector<unsigned short> &numIndicesPerMesh){
    
    // Main drawing loop
    
    glm::vec3 lightPositionWorld = glm::vec3(0.0f, 7.0f, 0.0f);
    
    float step = 0.05;
    glm::vec3 camera = glm::vec3(0.0f, 5.0f, 6.0f);
    glm::vec3 p = glm::vec3(1,0,0);       // +Y-axis of camera (basis vector) - up
    glm::vec3 q = glm::vec3(0,1,0);       // +X-axis of camera (basis vector) - right
    glm::vec3 r = glm::vec3(0,0,-1);      // -Z-axis of camera (basis vector) - front
    
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 )
    {
        // Move forward or back
        // Projection of vector onto plane explanation: https://www.maplesoft.com/support/help/Maple/view.aspx?path=MathApps/ProjectionOfVectorOntoPlane
        
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
        float angle = 0.01; // Increment to look up or down by
        
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
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen to dark blue, also depth buffer
        
        glEnable(GL_DEPTH_TEST);
        
        glUseProgram(programID); // Use the shader program to do the drawing
        
        //glm::mat4 myModelMatrix = myTranslationMatrix * myRotationMatrix * myScaleMatrix;
        //glm::mat4 myTranslationMatrix = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
        //glm::mat4 myScalingMatrix = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
        //glm::mat4 myRotationMatrix = glm::mat4();
        
        glm::mat4 viewMatrix = glm::lookAt(
                                           camera, // position of camera
                                           camera + r, // where to look
                                           glm::vec3(0.0f, 1.0f, 0.0f)    // +Y axis points up
                                           );
        
        //std::cout << "Camera position: " << camera.x << ", " << camera.y << ", " << camera.z << "\n";
        
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)1.0, 0.1f, 1000.0f);
        
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]); // Locn, count, transpose, value
        glUniform3f(LightPositionID, lightPositionWorld.x, lightPositionWorld.y, lightPositionWorld.z);
        glUniform3f(CameraPositionID, camera.x, camera.y, camera.z);
        
        for (int i = 0; i < numMeshes; i++)
        {
            // Bind our texture in Texture Unit 0
            glActiveTexture(GL_TEXTURE0);
            
            // Get the right texture by determining which model the mesh is in. The lookup numbers in the
            // textureIndices array are for each model. So an index of 0 in the part of the textureIndices
            // array that is for the second model (in the range of meshes belonging to the second model)
            // has to be adjusted (bumped up by the number of meshes found in the first model, roughly)
            // before lookup occurs in the master textures array. j represents number of the model.
            
            int modelNumber = 0;
            
            // Figure out which model we are in - the first one (j = 0) or a later one (j = 1, j = 2, etc.)
            
            while (cumulativeMeshes[modelNumber] < i+1)
            {
                modelNumber++;
            }
            
            if (modelNumber == 0) // No need to adjust index before lookup
            {
                //std::cout << "In first branch " << textureIndices[i] << "\n";
                glBindTexture(GL_TEXTURE_2D, textures[textureIndices[i]]);
            }
            else // Adjust index before lookup in textures according to number of preceding meshes
            {
                //std::cout << "In second branch " << textureIndices[i]+cumulativeTextures[j-1] << "\n";
                glBindTexture(GL_TEXTURE_2D, textures[textureIndices[i]+cumulativeTextures[modelNumber-1]]);
            }
            
            glm::mat4 myModelMatrix = translations[modelNumber] * rotations[modelNumber] * scales[modelNumber];
            //glm::mat4 myModelMatrixInverseTranspose = glm::transpose(glm::inverse(glm::mat3(translations[modelNumber] * rotations[modelNumber] * scales[modelNumber])));
            glm::mat4 mymatrix = projectionMatrix * viewMatrix * myModelMatrix;
            
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mymatrix[0][0]); // Sending the matrix to the shader
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &myModelMatrix[0][0]);
            //glUniformMatrix4fv(ModelMatrixInverseTransposeID, 1, GL_FALSE, &myModelMatrixInverseTranspose[0][0]);
            
            
            // Set our "myTextureSampler" sampler to user Texture Unit 0
            glUniform1i(textureID, 0);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer[i]);
            
            // 1st attribute buffer: locations of vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
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
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[i]);
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
            glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[i]);
            glVertexAttribPointer(
                                  2,                                // attribute
                                  3,                                // size
                                  GL_FLOAT,                         // type
                                  GL_FALSE,                         // normalized?
                                  0,                                // stride
                                  (void*)0                          // array buffer offset
                                  );
            
            glDrawElements(GL_TRIANGLES, numIndicesPerMesh[i]*3, GL_UNSIGNED_SHORT, (void*)0);
            //glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
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
    
    GLFWwindow * window = nullptr;
    window = glfwCreateWindow( 500, 500, "Drawing a Cube", NULL, NULL);
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
    
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    // Turn on culling; cull triangles with their back facing the camera
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Turn on z-buffering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);   // Keep the fragment w/shorter distance to camera
    
    // What is the point of this? As far as I can tell, it does not hold anything important, but no
    // triangles will be drawn if it is commented out. Does it create the context that we attach
    // attributes to in some way?
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);     // This is the VAO
    
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
    
    // This section is getting handles for uniforms in the shader
    GLuint MatrixID = glGetUniformLocation(programID, "MY_MATRIX");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "VIEW_MATRIX");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "MODEL_MATRIX");
    GLuint LightPositionID = glGetUniformLocation(programID, "LIGHT_POSITION_WORLDSPACE");
    GLuint CameraPositionID = glGetUniformLocation(programID, "CAMERA_POSITION_WORLDSPACE");
    //GLuint ModelMatrixInverseTransposeID = glGetUniformLocation(programID, "MODEL_MATRIX_INVERSE_TRANSPOSE");
    
    // Load the file that tells us what models we will have in the world and what their initial
    // transformations will be.
    
    std::vector<std::string> filenames;
    std::vector<glm::mat4> translations;
    std::vector<glm::mat4> scales;
    std::vector<glm::mat4> rotations;
    
    loadWorld("world1.txt", filenames, translations, scales, rotations);
    int numModels = filenames.size();
    
    // Vectors that we will pass into loadAssImp and get back filled with relevant data for the scene to be
    // drawn.
    
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texture_uvs;
    std::vector<glm::vec3> vertex_normals;
    std::vector<unsigned short> indexedVertices;
    std::vector<unsigned short> numVerticesPerMesh;
    std::vector<unsigned short> numIndicesPerMesh;
    std::vector<aiString> texturePaths;
    std::vector<unsigned short> textureIndices;
    std::vector<unsigned short> numTexturesPerModel;
    
    int numMeshes = 0;
    int meshesPerModel[numModels];
    int cumulativeMeshes[numModels];
    
    for (int i = 0; i < numModels; i++)
    {
        meshesPerModel[i] = loadAssImp(filenames[i], vertices, texture_uvs, vertex_normals, indexedVertices, numVerticesPerMesh, numIndicesPerMesh, texturePaths, textureIndices, numTexturesPerModel);
        numMeshes += meshesPerModel[i];
        cumulativeMeshes[i] = numMeshes;
        
    }
    
    // Load the textures specified by the texture paths we got from Assimp and make them OpenGL textures;
    // store the OpenGL handles for each texture we create in the textures array.
    
    std::vector<GLuint> textures; // To store texture handles once loaded from texturePaths obtained above
    
    // This will give us back a bunch of OpenGL-internal texture handles for the BMPs we have loaded
    // (which are now OpenGL textures that we must call by these handles).
    for (int i = 0; i < texturePaths.size(); i++)
    {
        textures.push_back(loadBMP(texturePaths[i].C_Str()));
        std::cout << texturePaths[i].C_Str() << " \n";
    }
    
    unsigned short cumulativeTextures[numModels];
    
    int total = 0;
    
    for (int i = 0; i < numTexturesPerModel.size(); i++)
    {
        //std::cout << numTexturesPerModel[i] << "\n";
        total += numTexturesPerModel[i];
        cumulativeTextures[i] = total;
    }
    
    // Get a handle for our "myTextureSampler" uniform
    GLuint textureID = glGetUniformLocation(programID, "myTextureSampler");
    
    // We will make a buffer of each type (vertex, uv, normal, and index) for each mesh. The data is not
    // actually split up in memory; we just tell each buffer to start reading in at the correct address in
    // memory. For instance, all the vertices in all the meshes of the scene are in a single std::vector.
    // However, they are copied into separate buffers in OpenGL by the code below. The offset variable is
    // used to bump our position in the std::vector array forward by the correct amount with each pass.
    
    GLuint vertexbuffer[numMeshes];
    glGenBuffers(numMeshes, vertexbuffer);
    int offset = 0;
    for (int i = 0; i < numMeshes; i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, numVerticesPerMesh[i]*sizeof(float)*3, &vertices[0+offset], GL_STATIC_DRAW);
        offset += numVerticesPerMesh[i];
    }
    
    // Same as above for uv (texture) coordinates
    GLuint uvbuffer[numMeshes];
    glGenBuffers(numMeshes, uvbuffer);
    offset = 0;
    for (int i = 0; i < numMeshes; i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, numVerticesPerMesh[i]*sizeof(float)*2, &texture_uvs[0+offset], GL_STATIC_DRAW);
        offset += numVerticesPerMesh[i];
    }
    
    // Same as above for vertex normals
    GLuint normalbuffer[numMeshes];
    glGenBuffers(numMeshes, normalbuffer);
    offset = 0;
    for (int i = 0; i < numMeshes; i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, numVerticesPerMesh[i]*sizeof(float)*3, &vertex_normals[0+offset], GL_STATIC_DRAW);
        offset += numVerticesPerMesh[i];
    }

    // This set of buffers consists of element array buffers (one for each mesh). These buffers contain
    // indices. Because they have a type of unsigned short (multiples of 1) and not a type of vec3
    // (multiples of 3), we need to multiply by three to get the correct offset, since there are actually
    // three unsigned shorts (3x the size in memory) for each index in the mesh.
    
    GLuint indexbuffer[numMeshes];
    glGenBuffers(numMeshes, indexbuffer);
    offset = 0;
    for (int i = 0; i < numMeshes; i++)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndicesPerMesh[i]*sizeof(unsigned short)*3, &indexedVertices[0+offset], GL_STATIC_DRAW);
        offset += numIndicesPerMesh[i]*3; // Has to be a 3 - these are just ints, not vecs
    }
    
    drawWorldOnLoop(window, programID, ViewMatrixID, LightPositionID, CameraPositionID, numMeshes, cumulativeMeshes, textures, textureIndices, cumulativeTextures, translations, rotations, scales, MatrixID, ModelMatrixID, textureID, vertexbuffer, normalbuffer, uvbuffer, indexbuffer,numIndicesPerMesh);
    
    // Cleanup VBO and shader
    
    for (int i = 0; i < numMeshes; i++)
    {
        glDeleteBuffers(1, &vertexbuffer[i]);
        glDeleteBuffers(1, &uvbuffer[i]);
        glDeleteBuffers(1, &normalbuffer[i]);
        glDeleteBuffers(1, &indexbuffer[i]);
    }
    glDeleteProgram(programID);
    glDeleteTextures(1, &textureID);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
    
}