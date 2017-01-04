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
#include <FreeImage.h>

using namespace glm;    // Allows us to say vec3 rather than glm::vec3

// Shader loading and compiling function

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
    
    // Create the shaders. We get back their IDs from OpenGL.
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Read the Vertex Shader code from the file, where it is stored as text.
    std::string VertexShaderCode; //String to hold what we read from the file.
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in); // Stream from file
    if(VertexShaderStream.is_open()){
        std::string Line = "";
        while(getline(VertexShaderStream, Line))    // Keep getting lines from file while it has more
            VertexShaderCode += "\n" + Line;        // Add line breaks between lines
        VertexShaderStream.close();                 // No more lines - close.
    }else{
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        getchar();
        return 0;
    }
    
    // Read the Fragment Shader code from the file. Same process as for vertex shader, but sep. variables.
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
    
    GLint Result = GL_FALSE;
    int InfoLogLength;
    
    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str(); // Make code a c_str (char array) and get pointer to its beginning
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL); // source of the shader with ID stored in VertextShader ID is the char array in VertexSourcePointer. ?-Why are we using a reference here?
    glCompileShader(VertexShaderID); // Compile the shader with given ID, for which we now have a source.
    
    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result); // Store compile status in Result (how used?)
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength); // Store log length in InfoLogLength
    
    // If there is anything in the log, that means there was an error
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }
    
    // Compile Fragment Shader (same as for vertex shader)
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
    
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }
    
    // Link the program. We create a program (getting its ID back from OpenGL), attach both shaders to it,
    // and then link (something that is called on the program, not the individual shaders).
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
    
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }
    
    // Detach the shaders from the program - we already have their linked output in the program, presumably.
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
    
    // Delete the shaders as we are done using them.
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    
    return ProgramID; // Return the id of the program so we can access it in other parts of our code.
}

//Load bitmap for texture (does not work any better if the one from the tutorial is used)

GLuint loadBMP(const char * imagepath)
{
    FIBITMAP *bitmap = FreeImage_Load(FIF_BMP, imagepath);
    
    //Conversion from FIBITMAP to bytes from: https://solarianprogrammer.com/2013/05/17/opengl-101-textures/
    
    BYTE *bitmapBytes = (BYTE*)FreeImage_GetBits(bitmap);
    
    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap), 0, GL_BGRA, GL_UNSIGNED_BYTE, bitmapBytes);
    
    std::cout << "FreeImage_GetWidth: " << FreeImage_GetWidth(bitmap) << ", FreeImage_GetHeight: " << FreeImage_GetHeight(bitmap) << "\n";
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // OpenGL has now copied the data. Free our own version
    FreeImage_Unload(bitmap);
    
    return textureID;
}

// Main function for drawing

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
    
    // This is tying the MatrixId to MY_MATRIX variable in the shader
    GLuint MatrixID = glGetUniformLocation(programID, "MY_MATRIX");
    
    // Load the texture
    GLuint texture = loadBMP("catfamily.bmp");
    
    // Get a handle for our "myTextureSampler" uniform
    GLuint textureID  = glGetUniformLocation(programID, "myTextureSampler");
    
    // An array of 3 vectors which represents 3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
        0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };
    
    // An array specifying which vertices to use for each triangle
    static const GLuint g_index_buffer_data[] = {
        0, 1, 2,
        3, 1, 0
    };
    
    // Two UV coordinatesfor each vertex. They were created with Blender.
    static const GLfloat g_uv_buffer_data[] = {
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f
    };
    
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    GLuint indexbuffer;
    glGenBuffers(1, &indexbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , indexbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_index_buffer_data), g_index_buffer_data, GL_STATIC_DRAW);
    
    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
    
    // Main drawing loop
    
    float scaleFactor = 1.0;
    glm::vec3 camera = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 lookAt = glm::normalize(glm::vec3(0, 0, -1)); // This needs to be (-) z vector b/c we are looking towards the back
    
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 )
    {
        // Strafe sideways
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            camera.z -= scaleFactor/100;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            camera.z += scaleFactor/100;
        }
        
        // Move forward or back
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera.x += scaleFactor/100;
            // TESTING THE VERSION CONTROL
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera.x -= scaleFactor/100;
        }
        
        glm::mat3 rotation = glm::mat3();
        float angle = 0.01;
        
        //Turn viewer's gaze up or down - should be rotate about x, but seems to be z?
        
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            //Rotate about x-axis
            
            if (glm::dot(glm::vec3(0, 1, 0), lookAt) > -0.99)
            {
                rotation[0] = glm::vec3(1, 0, 0);
                rotation[1] = glm::vec3(0, cos(-angle), sin(-angle));
                rotation[2] = glm::vec3(0, -sin(-angle), cos(-angle));
            }
            
        }
        else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            //Rotate about x-axis
            if (glm::dot(glm::vec3(0, 1, 0), lookAt) < 0.99)
            {
                rotation[0] = glm::vec3(1, 0, 0);
                rotation[1] = glm::vec3(0, cos(angle), sin(angle));
                rotation[2] = glm::vec3(0, -sin(angle), cos(angle));
            }
        }
        
        // Turn viewer's gaze and direction of motion to the side
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            //rotate about y-axis
            rotation[0] = glm::vec3(cos(-angle), 0, -sin(-angle));
            rotation[1] = glm::vec3(0, 1, 0);
            rotation[2] = glm::vec3(sin(-angle), 0, cos(-angle));
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            //rotate about y-axis
            rotation[0] = glm::vec3(cos(angle), 0, -sin(angle));
            rotation[1] = glm::vec3(0, 1, 0);
            rotation[2] = glm::vec3(sin(angle), 0, cos(angle));
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen to dark blue, also depth buffer
        
        glUseProgram(programID); // Use the shader program to do the drawing
        
        //glm::mat4 myModelMatrix = myTranslationMatrix * myRotationMatrix * myScaleMatrix;
        glm::mat4 myTranslationMatrix = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 myScalingMatrix = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
        glm::mat4 myRotationMatrix = glm::mat4();
        glm::mat4 myModelMatrix = myTranslationMatrix * myRotationMatrix * myScalingMatrix;
        
        lookAt = rotation * lookAt;
        
        glm::mat4 viewMatrix = glm::lookAt(
                                             camera, // position of camera
                                             camera + lookAt, // where to look
                                             glm::vec3(0.0f, 1.0f, 0.0f)    // +Y axis points up
                                             );
        
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)1.0, 0.1f, 1000.0f);
        glm::mat4 mymatrix = projectionMatrix * viewMatrix * myModelMatrix;
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mymatrix[0][0]); // Sending the matrix to the shader
        
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(textureID, 0);
        
        // 1st attribute buffer: locations of vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
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
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
                              1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                              2,                                // size : U+V => 2
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &textureID);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
    
}