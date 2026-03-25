#include <glfw/glfw3.h>
#include <map>
#include "paint.h"

struct Coordenada {
    int x, z;
    bool operator < (const Coordenada& other) const {
        if (x != other.x) return x < other.x;
        return z < other.z;
    }
};

class chunkLoader {
        std::map<Coordenada, Paint*> mundialMap; 
        Shader* wallShader; Texture* wallTexture;

        chunkLoader(Shader* wallShader, Texture* wallTexture){
            this->wallShader = wallShader;
            this->wallTexture = wallTexture;
        }

        void loadChunk(GLFWwindow *window, Camera &cam) {
        float escala = 45.0f;
        int cellx = (int)floor(cam.position.x / escala), cellz = (int)floor(cam.position.z / escala), radio = 2;

        static int lastCellX = -99999, lastCellZ = -99999;
        if (lastCellX != -99999) {
            if (cellx > lastCellX) {
                int badX = lastCellX + radio;
                for (auto it = mundialMap.begin(); it != mundialMap.end(); ) {
                    if (it->first.x == badX) {
                        delete it->second;
                        it = mundialMap.erase(it);
                    } else ++it;
                }
            }
            if (cellz > lastCellZ) {
                int badZ = lastCellZ + radio;
                for (auto it = mundialMap.begin(); it != mundialMap.end(); ) {
                    if (it->first.z == badZ) {
                        delete it->second;
                        it = mundialMap.erase(it);
                    } else ++it;
                }
            }
        }
        lastCellX = cellx; lastCellZ = cellz;

        std::map<Coordenada, Model*> tempModels;

        // FASE 1: Crear modelos lógicos
        for (int x = cellx - radio; x <= cellx + radio; x++) {
            for (int z = cellz - radio; z <= cellz + radio; z++) {
                Coordenada c = {x, z};
                Model* m = new Model();
                m->processSeed(10 + x * 10 + z, x, z);
                tempModels[c] = m;
            }
        }

        // FASE 2: Conectar
        for (int x = cellx - radio; x <= cellx + radio; x++) {
            for (int z = cellz - radio; z <= cellz + radio; z++) {
                Coordenada coord = {x, z};
                // Usamos la informacion REAL del modelo, no la funcion raw connectRight
                if (tempModels.count({x+1, z}) && tempModels[coord]->doorData.count(0)) {
                    DoorPoints p1 = tempModels[coord]->doorData[0], p2 = tempModels[{x+1, z}]->doorData[1];
                    p2.infIzq.x += escala; p2.supIzq.x += escala; p2.infDer.x += escala; p2.supDer.x += escala;
                    tempModels[coord]->generateCorridor(p1, p2);
                }
                if (tempModels.count({x, z+1}) && tempModels[coord]->doorData.count(2)) {
                    DoorPoints p1 = tempModels[coord]->doorData[2], p2 = tempModels[{x, z+1}]->doorData[3];
                    p2.infIzq.z += escala; p2.supIzq.z += escala; p2.infDer.z += escala; p2.supDer.z += escala;
                    tempModels[coord]->generateCorridor(p1, p2);
                }
                // FASE 3: Subir a GPU
                if (mundialMap.find(coord) == mundialMap.end()) {
                    tempModels[coord]->updatePointers();
                    Paint* newRoom = new Paint("pared.jpg", wallShader, wallTexture);
                    newRoom->configBuffers(*tempModels[coord]);
                    mundialMap[coord] = newRoom;
                }
                mundialMap[coord]->draw(glm::vec3(x * escala, 0.0f, z * escala));
            }
        }

        // Limpieza de punteros segura (C++11 compatible)
        for (auto const& it : tempModels) {
            delete it.second;
        }

        static int cleanup = 0;
        if (++cleanup > 400) {
            cleanup = 0;
            for (auto it = mundialMap.begin(); it != mundialMap.end(); ) {
                if (abs(it->first.x - cellx) > 5 || abs(it->first.z - cellz) > 5) {
                    delete it->second; it = mundialMap.erase(it);
                } else ++it;
            }
        }
    }
};