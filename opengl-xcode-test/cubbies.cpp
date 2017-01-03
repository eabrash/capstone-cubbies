//
//  main.cpp
//
// Based on code from opengl-tutorial: http://www.opengl-tutorial.org/download/ (licensed under FTWPL).
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
    
    // What is the point of this? As far as I can tell, it does not hold anything important, but no
    // triangles will be drawn if it is commented out. Does it create the context that we attach
    // attributes to in some way?
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);     // This is the VAO
    
    // An array of 3 vectors which represents 3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };
    
    //int numPts = 36;
    
    GLuint vertexbuffer;   // This will identify our vertex buffer
    glGenBuffers(1, &vertexbuffer); // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
                          0,
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    // Now handle the color buffer
    
    static const GLfloat g_color_buffer_data [] = {
        0.583f,  0.583f,  0.583f,
        0.583f,  0.583f,  0.583f,
        0.583f,  0.583f,  0.583f,
        0.583f,  0.583f,  0.583f,
        0.583f,  0.583f,  0.583f,
        0.583f,  0.583f,  0.583f,
        0.583f,  0.583f,  0.583f,
        0.583f,  0.583f,  0.583f,
        0.583f,  0.583f,  0.583f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };
    
    GLuint colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer(
                          1,
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
    
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    // Only during the initialisation
    GLuint MatrixID = glGetUniformLocation(programID, "MY_MATRIX"); // This is tying the MatrixId to MY_MATRIX variable in the shader
    
    // Turn on culling; cull triangles with their back facing the camera
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Turn on z-buffering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);   // Keep the fragment w/shorter distance to camera
    
    // Main drawing loop
    
    //int counter = 0;
    
    float scaleFactor = 20.0;
    
    
    glm::vec3 camera = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 lookAt = glm::vec3(-1, 0, 0);
    
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 )
    {
        // Strafe sideways
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera.z -= scaleFactor/100;
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera.z += scaleFactor/100;
        }
        
        // Move forward or back
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            camera.x += scaleFactor/100;
        }
        else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            camera.x -= scaleFactor/100;
        }
        
        glm::mat3 rotation = glm::mat3();
        float angle = 0.01;
        
        //Turn viewer's gaze up or down - should be rotate about x, but seems to be z?
        
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            //Rotate about z-axis (why is it the z-axis?)
            
            if (glm::dot(glm::vec3(0, 1, 0), lookAt) > -0.99)
            {
                rotation[0] = glm::vec3(cos(angle), sin(angle), 0);
                rotation[1] = glm::vec3(-sin(angle), cos(angle), 0);
                rotation[2] = glm::vec3(0, 0, 1);
            }
            
            //Rotate about x-axis
            
//            rotation[0] = glm::vec3(1, 0, 0);
//            rotation[1] = glm::vec3(0, cos(angle), sin(angle));
//            rotation[2] = glm::vec3(0, -sin(angle), cos(angle));
            
        }
        else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            //Rotate about z-axis (why is it the z-axis?)
            
            if (glm::dot(glm::vec3(0, 1, 0), lookAt) < 0.99)
            {
                rotation[0] = glm::vec3(cos(-angle), sin(-angle), 0);
                rotation[1] = glm::vec3(-sin(-angle), cos(-angle), 0);
                rotation[2] = glm::vec3(0, 0, 1);
            }
            
//            //Rotate about x-axis
//            rotation[0] = glm::vec3(1, 0, 0);
//            rotation[1] = glm::vec3(0, cos(angle), sin(angle));
//            rotation[2] = glm::vec3(0, -sin(angle), cos(angle));
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
        glm::mat4 myTranslationMatrix = glm::translate(glm::vec3(-scaleFactor, 0.0f, scaleFactor));
        glm::mat4 myScalingMatrix = glm::scale(glm::vec3(scaleFactor, scaleFactor, scaleFactor/50));
        glm::mat4 myRotationMatrix = glm::mat4();
        glm::mat4 myModelMatrix = myTranslationMatrix * myRotationMatrix * myScalingMatrix;
        
        lookAt = rotation * lookAt;
        
        glm::mat4 viewMatrix = glm::lookAt(
                                             camera, // position of camera
                                             camera + lookAt, // where to look
                                             glm::vec3(0.0f, 1.0f, 0.0f)    // +Y axis points up
                                             );
        
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), (float)1.0, 0.1f, 1000.0f);
        glm::mat4 mymatrix = projectionMatrix * viewMatrix * myModelMatrix;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mymatrix[0][0]); // This is sending the matrix to the shader
        
        //        glEnableVertexAttribArray(0);
        //        glEnableVertexAttribArray(1);
        
        glDrawArrays(GL_TRIANGLES, 0, 12*3);
        
        myTranslationMatrix = glm::translate(glm::vec3(-scaleFactor, 0.0f, -scaleFactor));
        myModelMatrix = myTranslationMatrix * myRotationMatrix * myScalingMatrix;
        mymatrix = projectionMatrix * viewMatrix * myModelMatrix;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mymatrix[0][0]); // GL_FALSE says "not transpose". Not sure what the 1 indicates.
        glDrawArrays(GL_TRIANGLES, 0, 12*3);
        
        //        glDisableVertexAttribArray(0); // Why do we need to do this?
        //        glDisableVertexAttribArray(1);

        // Floor
        
        myTranslationMatrix = glm::translate(glm::vec3(-scaleFactor, -scaleFactor, 0.0f));
        myRotationMatrix = glm::rotate(glm::radians(90.0f), glm::vec3(1, 0, 0));
        myModelMatrix = myTranslationMatrix * myRotationMatrix * myScalingMatrix;
        mymatrix = projectionMatrix * viewMatrix * myModelMatrix;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mymatrix[0][0]); // GL_FALSE says "not transpose". Not sure what the 1 indicates.
        glDrawArrays(GL_TRIANGLES, 0, 12*3);
        
        // Ceiling
        
        myTranslationMatrix = glm::translate(glm::vec3(-scaleFactor, scaleFactor, 0.0f));
        myRotationMatrix = glm::rotate(glm::radians(90.0f), glm::vec3(1, 0, 0));
        myModelMatrix = myTranslationMatrix * myRotationMatrix * myScalingMatrix;
        mymatrix = projectionMatrix * viewMatrix * myModelMatrix;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mymatrix[0][0]); // GL_FALSE says "not transpose". Not sure what the 1 indicates.
        glDrawArrays(GL_TRIANGLES, 0, 12*3);
        
        // Back wall
        
        myTranslationMatrix = glm::translate(glm::vec3(-scaleFactor*2, 0.0f, 0.0f));
        myRotationMatrix = glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0));
        myModelMatrix = myTranslationMatrix * myRotationMatrix * myScalingMatrix;
        mymatrix = projectionMatrix * viewMatrix * myModelMatrix;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mymatrix[0][0]); // GL_FALSE says "not transpose". Not sure what the 1 indicates.
        glDrawArrays(GL_TRIANGLES, 0, 12*3);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }
    
    return 0;
    
}