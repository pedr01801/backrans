#ifndef TEXTURE_H    
#define TEXTURE_H     
#include <std/stb_image.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "shader.h"

class Texture 
{
    public:
        int width, height, chanels;
        GLuint texture;

        Texture(const char* name, Shader *shader){
            stbi_set_flip_vertically_on_load(true);

            unsigned char* bytes = stbi_load(name, &width, &height, &chanels, 4);

            glGenTextures(1, &texture);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(bytes);

            shader->use();
            shader->setInt("text", 0);
        }

        void use(int unit = 0) {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, texture);
        }
        
};

#endif