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

int SCR_WIDTH = 800;
int SCR_HEIGHT = 800;

bool firstMouse = true;
float xlast = 0, ylast = 0, dt = 0, lastFrame = 0;

// Declaraciones
GLFWwindow *start(int width, int height, const char* title);
void processKeyboardInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void loadChunk(GLFWwindow *window);

// Funciones externas definidas en model.cpp
bool connectRight(int x, int z);
bool connectUp(int x, int z);

struct Coordenada {
    int x, z;
    bool operator < (const Coordenada& other) const {
        if (x != other.x) return x < other.x;
        return z < other.z;
    }
};

struct RoomSize { float w, d; };

std::map<Coordenada, Paint*> mundialMap; 
std::map<Coordenada, RoomSize> roomSizes;

Camera cam;
Shader* wallShader;
Texture* wallTexture;

int main() {   
    GLFWwindow* window = start(SCR_WIDTH, SCR_HEIGHT, "Backrooms - Rescate Final");
    if (!window) return -1;

    wallShader = new Shader("3.3.shader.vs", "3.3.shader.fs");
    wallTexture = new Texture("pared.jpg", wallShader);

    while(!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        wallShader->use();
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/SCR_HEIGHT, 0.1f, 1000.0f);
        wallShader->setMat4("projection", proj);
        wallShader->setMat4("view", cam.getViewMatrix());

        loadChunk(window);
        processKeyboardInput(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}

void loadChunk(GLFWwindow *window) {
    float escala = 45.0f; 
    int cellx = (int)floor(cam.position.x / escala);
    int cellz = (int)floor(cam.position.z / escala);
    int radio = 2;

    // 1. Generamos modelos temporales para el área actual
    std::map<Coordenada, Model*> tempModels;
    for (int x = cellx - radio; x <= cellx + radio; x++) {
        for (int z = cellz - radio; z <= cellz + radio; z++) {
            Coordenada c = {x, z};
            Model* m = new Model();
            m->processSeed(10 + x * 10 + z, x, z);
            tempModels[c] = m;
        }
    }

    // 2. Conectamos los pasillos
    for (int x = cellx - radio; x <= cellx + radio; x++) {
        for (int z = cellz - radio; z <= cellz + radio; z++) {
            Coordenada coord = {x, z};
            
            // Pasillo a la derecha (+X)
            if (connectRight(x, z)) {
                Coordenada vecino = {x + 1, z};
                if (tempModels.count(vecino)) {
                    DoorPoints p1 = tempModels[coord]->doorData[0];
                    DoorPoints p2 = tempModels[vecino]->doorData[1];
                    
                    // Proyectamos p2 a la posición relativa del vecino
                    p2.infIzq.x += escala; p2.supIzq.x += escala;
                    p2.infDer.x += escala; p2.supDer.x += escala;
                    
                    tempModels[coord]->generateCorridor(p1, p2);
                }
            }

            // Pasillo arriba (+Z)
            if (connectUp(x, z)) {
                Coordenada vecino = {x, z + 1};
                if (tempModels.count(vecino)) {
                    DoorPoints p1 = tempModels[coord]->doorData[2];
                    DoorPoints p2 = tempModels[vecino]->doorData[3];
                    
                    p2.infIzq.z += escala; p2.supIzq.z += escala;
                    p2.infDer.z += escala; p2.supDer.z += escala;
                    
                    tempModels[coord]->generateCorridor(p1, p2);
                }
            }

            // 3. Renderizado y subida a GPU
            if (mundialMap.find(coord) == mundialMap.end()) {
                tempModels[coord]->updatePointers(); 
                Paint* newRoom = new Paint("pared.jpg", wallShader, wallTexture);
                newRoom->configBuffers(*tempModels[coord]);
                mundialMap[coord] = newRoom;
                roomSizes[coord] = { (float)tempModels[coord]->w, (float)tempModels[coord]->d };
            }

            mundialMap[coord]->draw(glm::vec3(x * escala, 0.0f, z * escala));
        }
    }

    // Limpieza de modelos lógicos temporales
    for (std::map<Coordenada, Model*>::iterator it = tempModels.begin(); it != tempModels.end(); ++it) {
        delete it->second;
    }

    // Fase de limpieza de memoria mundial (lejanía)
    static int frameCounter = 0;
    if (++frameCounter > 500) {
        frameCounter = 0;
        for (auto it = mundialMap.begin(); it != mundialMap.end(); ) {
            int dx = abs(it->first.x - cellx);
            int dz = abs(it->first.z - cellz);
            if (dx > 4 || dz > 4) {
                delete it->second; 
                roomSizes.erase(it->first);
                it = mundialMap.erase(it);
            } else {
                ++it;
            }
        }
    }
}

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
    glfwSwapInterval(1);
    return window;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { xlast = (float)xpos; ylast = (float)ypos; firstMouse = false; }
    float xoffset = (float)xpos - xlast;
    float yoffset = ylast - (float)ypos;
    xlast = (float)xpos; ylast = (float)ypos;
    cam.updateAngle(xoffset, yoffset);
}