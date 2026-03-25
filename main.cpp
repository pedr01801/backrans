#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "paint.h"
#include "camera.h"
#include "model.h"
#include "chunkLoader.h"

int SCR_WIDTH = 800; int SCR_HEIGHT = 800;
bool firstMouse = true; float xlast = 0, ylast = 0, dt = 0, lastFrame = 0;

// Declaraciones de funciones de GLFW
GLFWwindow *start(int width, int height, const char* title);
void processKeyboardInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Variables globales necesarias para la cámara y el renderizado básico
Camera cam; 
Shader* wallShader; 
Texture* wallTexture;

int main() {   
    // Inicialización de ventana
    GLFWwindow* window = start(SCR_WIDTH, SCR_HEIGHT, "Backrooms - ChunkLoader Class");
    if (!window) return -1;

    // Carga de recursos
    wallShader = new Shader("3.3.shader.vs", "3.3.shader.fs");
    wallTexture = new Texture("pared.jpg", wallShader);

    // INSTANCIA DE LA CLASE: Aquí creamos el gestor de niveles
    chunkLoader *chunki = new chunkLoader(wallShader, wallTexture);

    while(!glfwWindowShouldClose(window)) {
        // Cálculo del Delta Time
        float currentFrame = glfwGetTime(); 
        dt = currentFrame - lastFrame; 
        lastFrame = currentFrame;

        // Renderizado
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        wallShader->use();
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/SCR_HEIGHT, 0.1f, 1000.0f);
        wallShader->setMat4("projection", proj);
        wallShader->setMat4("view", cam.getViewMatrix());

        // --- USO DE LA CLASE ---
        // Llamamos al método de la clase pasando la cámara actual
        chunki->loadChunk(window, cam);

        processKeyboardInput(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpieza
    delete chunki;
    delete wallShader;
    delete wallTexture;

    glfwTerminate();
    return 0;
}

// Implementación de Input y Callbacks
void processKeyboardInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cam.updatePos(FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam.updatePos(BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam.updatePos(LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam.updatePos(RIGHT, dt);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cam.updatePos(FLY, dt);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) cam.updatePos(DOWN, dt);
}

GLFWwindow *start(int width, int height, const char* title) {
    if (!glfwInit()) return nullptr;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) return nullptr;

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return nullptr;

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1); // V-Sync activo para evitar parpadeos
    return window;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { xlast = (float)xpos; ylast = (float)ypos; firstMouse = false; }
    float xoffset = (float)xpos - xlast;
    float yoffset = ylast - (float)ypos;
    xlast = (float)xpos; ylast = (float)ypos;
    cam.updateAngle(xoffset, yoffset);
}