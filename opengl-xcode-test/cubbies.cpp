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

using namespace glm;    // Allows us to say vec3 rather than glm::vec3

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

// Load bitmap for texture. This method uses the FreeImage library for loading. FreeImage can load various
// image types, but this function is only intended for BMP files.

GLuint loadBMP(const char * imagepath)
{
    FIBITMAP *bitmap = FreeImage_Load(FIF_BMP, imagepath);
    
    //Conversion from FIBITMAP to bytes from: https://solarianprogrammer.com/2013/05/17/opengl-101-textures/
    
    BYTE *bitmapBytes = (BYTE*)FreeImage_GetBits(bitmap);
    
    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    // "Bind" the newly created texture : all future texture functions will modify this texture.
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

// These arrays will hold the data that is given to OpenGL to draw

std::vector<unsigned short> indices;
std::vector<glm::vec3> vertices;
std::vector<glm::vec2> texture_uvs;
std::vector<glm::vec3> vertex_normals;


//Get the number of meshes in a model

int getNumMeshes (char *filename)
{
    Assimp::Importer importer;
    
    const aiScene *scene = importer.ReadFile(filename, aiProcess_JoinIdenticalVertices);
    
    return scene->mNumMeshes;
}

// Code modified from opengl-tutorials (http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/, see associated code for tutorial 09 in assimp version)
// and from http://www.assimp.org/lib_html/usage.html

void loadAssImp(char *filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals, std::vector<unsigned short> &indices, std::vector<unsigned short> &verticesPerMesh, std::vector<unsigned short> &indicesPerMesh, std::vector<aiString> &texturePaths, std::vector<unsigned short> &textureIndices)
{
    Assimp::Importer importer;
    
    const aiScene *scene = importer.ReadFile(filename, aiProcess_JoinIdenticalVertices);
    
    int numMeshes = scene->mNumMeshes;
    
//    std::cout << "Num materials: " << scene->mNumMaterials << "\n";
//    std::cout << "Num Meshes: " << numMeshes << "\n";
//    std::cout << "Materials list: \n";
    
    std::vector<aiString> paths;
    
    int emptyCount = 0; // Number of empty textures encountered, usually at least one
    
    for (int i = 0; i < scene->mNumMaterials; i++)
    {
        aiString filepath;
        int index = 0;
        
        // From http://www.lighthouse3d.com/cg-topics/code-samples/importing-3d-models-with-assimp/
        
        scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, index, &filepath);
        paths.push_back(filepath);
        
//        std::cout << filepath.C_Str() << "\n";
//        std::cout << index << "\n";
        
        if (filepath.length != 0)
        {
            texturePaths.push_back(filepath);
        }
        else
        {
            emptyCount++;
        }
    }
    
    std::cout<< "Emptycount: " << emptyCount << "\n";
    
    for (int j = 0; j < numMeshes; j++)
    {
        aiMesh *mesh = scene->mMeshes[j];
        
        //std::cout << "Mesh " << j << ": material is " << mesh->mMaterialIndex << "\n";
        
        textureIndices.push_back(mesh->mMaterialIndex-emptyCount);
        
        //Get the pointers to the vertices, texture (uv) coords, and normals
        int numVertices = mesh->mNumVertices;
        aiVector3D *meshVertices = mesh->mVertices;
        aiVector3D *meshTextures = mesh->mTextureCoords[0];
        aiVector3D *meshNormals = mesh->mNormals;
        //aiVector3D *meshColors = mesh->mColors[0]; //We could also get colors if this had been in the .obj
        
        for (int i = 0; i < numVertices; i++)
        {
//            std::cout << "vertices: " << meshVertices[i].x << ", " << meshVertices[i].y << ", " << meshVertices[i].z << "\n";
//            std::cout << "textures: " << meshTextures[i].x << ", " << meshTextures[i].y << ", " << meshTextures[i].z << "\n";
//            std::cout << "normals: " << meshNormals[i].x << ", " << meshNormals[i].y << ", " << meshNormals[i].z << "\n";
//            std::cout << i << "\n";
//            
            vertices.push_back(glm::vec3(meshVertices[i].x, meshVertices[i].y, meshVertices[i].z));
            uvs.push_back(glm::vec2(meshTextures[i].x, meshTextures[i].y));
            normals.push_back(glm::vec3(meshNormals[i].x, meshNormals[i].y, meshNormals[i].z));
        }
        
        verticesPerMesh.push_back(numVertices);
        
        aiFace *meshFaces = mesh->mFaces;
        int numFaces = mesh->mNumFaces;
        
        for (int i = 0; i < numFaces; i++)
        {
            indices.push_back(meshFaces[i].mIndices[0]);
            indices.push_back(meshFaces[i].mIndices[1]);
            indices.push_back(meshFaces[i].mIndices[2]);
        }
        
        indicesPerMesh.push_back(numFaces);
    }
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
    
    // This section is getting handles for uniforms in the shader
    GLuint MatrixID = glGetUniformLocation(programID, "MY_MATRIX");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "VIEW_MATRIX");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "MODEL_MATRIX");
    GLuint LightPositionID = glGetUniformLocation(programID, "LIGHT_POSITION_WORLDSPACE");
    GLuint CameraPositionID = glGetUniformLocation(programID, "CAMERA_POSITION_WORLDSPACE");
    
//    char *filename1 = "tilebench.obj";
//    char *filename2 = "catsphere.obj";
//    char *filename3 = "walls5.obj";
    
    char *filenames [] = { "catsphere.obj", "woodinnertube.obj", "tilebench.obj", "walls5.obj"};
    int numModels = 4;
    
    int numMeshes = 0;
    int meshesPerModel[numModels];
    int cumulativeMeshes[numModels];
    
    std::cout << "Cumulative meshes:" << "\n";
    
    for (int i = 0; i < numModels; i++)
    {
        meshesPerModel[i] = getNumMeshes(filenames[i]);
        numMeshes += meshesPerModel[i];
        cumulativeMeshes[i] = numMeshes;
        std::cout << cumulativeMeshes[i] << "\n";
    }
    
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
    
    
    for (int i = 0; i < numModels; i++)
    {
        loadAssImp(filenames[i], vertices, texture_uvs, vertex_normals, indexedVertices, numVerticesPerMesh, numIndicesPerMesh, texturePaths, textureIndices);
    }
    std::vector<GLuint> textures; // To store texture handles once loaded from texturePaths obtained above
    
    // This will give us back a bunch of OpenGL-internal texture handles for the BMPs we have loaded
    // (which are now OpenGL textures that we must call by these handles).
    for (int i = 0; i < texturePaths.size(); i++)
    {
        textures.push_back(loadBMP(texturePaths[i].C_Str()));
        //std::cout << texturePaths[i].C_Str() << " \n";
    }
    
//    for (int i = 0; i < textureIndices.size(); i++)
//    {
//        std::cout << textureIndices[i] << "\n";
//    }
    
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
            glm::vec3 inCameraDirection = glm::mat3(p,q,r)*vec3(0,0,1);
            //std::cout << "inCameraDirection: " << inCameraDirection.x << inCameraDirection.y <<inCameraDirection.z << "\n";
            camera = camera + normalize(vec3(inCameraDirection.x, 0, inCameraDirection.z))*step;

        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            glm::vec3 inCameraDirection = glm::mat3(p,q,r)*vec3(0,0,1);
            //std::cout << "inCameraDirection: " << inCameraDirection.x << inCameraDirection.y <<inCameraDirection.z << "\n";
            camera = camera + normalize(vec3(inCameraDirection.x, 0, inCameraDirection.z))*(-step);
        }
        
        // Strafe sideways
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera = camera + glm::mat3(p,q,r)*vec3(-step,0,0);
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera = camera + glm::mat3(p,q,r)*vec3(step,0,0);
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
        glm::mat4 myTranslationMatrix = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 myScalingMatrix = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
        glm::mat4 myRotationMatrix = glm::mat4();
        glm::mat4 myModelMatrix = myTranslationMatrix * myRotationMatrix * myScalingMatrix;
        
        glm::mat4 viewMatrix = glm::lookAt(
                                             camera, // position of camera
                                             camera + r, // where to look
                                             glm::vec3(0.0f, 1.0f, 0.0f)    // +Y axis points up
                                             );
        
        //std::cout << "Camera position: " << camera.x << ", " << camera.y << ", " << camera.z << "\n";
        
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)1.0, 0.1f, 1000.0f);
        glm::mat4 mymatrix = projectionMatrix * viewMatrix * myModelMatrix;
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mymatrix[0][0]); // Sending the matrix to the shader
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]); // Locn, count, transpose, value
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &myModelMatrix[0][0]);
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
            // before lookup occurs in the master textures array, which contains texture IDs for all of
            // the textures in linear order. Note: number of textures in main texture array = number of
            // meshes under current import process. I.e., if the same texture is used for several
            // meshes, it will be read in twice. Not optimal long-term but works okay for now.
            
            int j = 0;
            
            // Figure out which model we are in - the first one (j = 0) or a later one (j = 1, j = 2, etc.)
            
            while (cumulativeMeshes[j] < i+1)
            {
                j++;
            }
            
            if (j==0) // No need to adjust index before lookup
            {
                //std::cout << "In first branch; i =" << i << "\n";
                glBindTexture(GL_TEXTURE_2D, textures[textureIndices[i]]);
            }
            else // Adjust index before lookup in textures according to number of preceding meshes
            {
                if (cumulativeMeshes[j-1] == 1 || cumulativeMeshes[j-1]-cumulativeMeshes[j-2] == 1)
                {
                    glBindTexture(GL_TEXTURE_2D, textures[textureIndices[i]+cumulativeMeshes[j-1]]);
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, textures[textureIndices[i]+cumulativeMeshes[j-1]-1]);
                }
            }
            
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