#ifndef PAINT_H     
#define PAINT_H      

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "model.h"

class Paint {
    public:
        GLuint VAO, VBO, EBO;
        Shader *shader;
        Texture *text;
        int cant;
        
        Paint(const char* texturePath, Shader *s, Texture *t){
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            shader = s;
            text = t;
        }

        void configBuffers(Model &room, bool hasEbO) 
        {
            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, room.sizeVertices, room.vertices, GL_STATIC_DRAW);

            if (hasEbO)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, room.sizeIndices, room.indices, GL_STATIC_DRAW);
            }
            
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            cant = room.indicesAmount;
            glBindVertexArray(0); 
        }

        void draw(glm::vec3 pos, float rot = 0.0f) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::rotate(model, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));
            
            shader->setMat4("model", model);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, text->texture);
            
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, cant, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        ~Paint(){
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }

        void reset() {
            cant = 0;  
        }
};

#endif