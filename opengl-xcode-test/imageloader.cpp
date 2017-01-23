// imageloader.cpp

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <FreeImage.h>

// Load BMP from file. Currently set to handle RGBA BMPs only, but FreeImage can handle various other
// file types, so this should be extendable without too much trouble.

GLuint loadBMP(const char * imagepath)
{
    FIBITMAP *bitmap = FreeImage_Load(FIF_BMP, imagepath);
    
    //Conversion from FIBITMAP to bytes from: https://solarianprogrammer.com/2013/05/17/opengl-101-textures/
    
    BYTE *bitmapBytes = (BYTE*)FreeImage_GetBits(bitmap);
    
    unsigned int bpp = FreeImage_GetBPP(bitmap);
    
    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    // "Bind" the newly created texture : all future texture functions will modify this texture.
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    //Use of BPP to differentiate RGB and RGBA images from
    //https://solarianprogrammer.com/2013/05/17/opengl-101-textures/
    
    if (bpp == 24)
    {
        // Give the image to OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap), 0, GL_BGR, GL_UNSIGNED_BYTE, bitmapBytes);
    }
    else if (bpp == 32)
    {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap), 0, GL_BGRA, GL_UNSIGNED_BYTE, bitmapBytes);
    }
    
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