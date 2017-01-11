#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Shader loading and compiling function (straight from opengl-tutorial.org)

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path){
    
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
