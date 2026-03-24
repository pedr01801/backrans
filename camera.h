#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "shader.h"

enum State {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    FLY,
    DOWN
};

class Camera {
    public:
        //Configuración shader
        Shader *shader;

        //Vectores
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right;
        
        //Matrices
        glm::mat4 view;

        //Variables auxiliares
        float SENSIVILITY = 0.1f;
        float yaw = 90.0f;
        float pitch = 0.0f;
        float speed = 25.0f;

        glm::mat4 getViewMatrix(){
            return glm::lookAt(position, position + front, up);
        }

        void updatePos(State state, float dt){
            glm::vec3 step = glm::normalize(glm::vec3(front.x, 0.0f, front.z));

            if (state == FORWARD){
                position += step * dt * speed;
            }

            if (state == BACKWARD){
                position -= step * dt * speed;
            }

            if (state == LEFT){
                position -= right * dt * speed;
            }

            if (state == RIGHT){
                position += right * dt * speed;
            }

            if (state == FLY){
                position.y += dt * speed;
            }

            if (state == DOWN){
                position.y -= dt * speed;
            }
        }

        void updateAngle(float xoffset, float yoffset){
            xoffset *= SENSIVILITY;
            yoffset *= SENSIVILITY;

            yaw = fmod(yaw + xoffset, 360.0f);
            pitch += yoffset;

            if(pitch > 89.0f)
            {
                pitch = 89.0f;
            }
            if(pitch < -89.0f)
            {
                pitch = -89.0f;
            }

            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

            front = glm::normalize(front);
            right = glm::normalize(glm::cross(front, up));
        }
};

#endif // CAMERA_H