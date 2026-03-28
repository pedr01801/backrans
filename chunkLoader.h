#ifndef CHUNKLOADER_H
#define CHUNKLOADER_H

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <map>
#include <vector>
#include <glm/glm.hpp>
#include <cmath>
#include "paint.h"
#include "camera.h"
#include "model.h"

#ifndef COORDENADA_STRUCT
#define COORDENADA_STRUCT
struct Coordenada {
    int x, z;
    bool operator < (const Coordenada& other) const {
        if (x != other.x) return x < other.x;
        return z < other.z;
    }
};
#endif

class chunkLoader {
public:
    std::map<Coordenada, Paint*> mundialMap; 
    
    std::map<Coordenada, Model*> dataMap;

    std::vector<Model*> modelPool;
    std::vector<Paint*> paintPool;
    
    Shader* wallShader; 
    Texture* wallTexture;

    chunkLoader(Shader* wallShader, Texture* wallTexture) {
        this->wallShader = wallShader;
        this->wallTexture = wallTexture;
        
        for (int i = 0; i < 50; i++)
        {
            modelPool.push_back(new Model());
            paintPool.push_back(new Paint("pared.jpg", wallShader, wallTexture));
        }
    }

    //Pasar a su propia clase
    Model* getModel() {
        if (modelPool.empty()) return new Model(); // Red de seguridad
        Model* m = modelPool.back();
        modelPool.pop_back();
        return m;
    }

    Paint* getPaint()
    {
        if (paintPool.empty()) return new Paint("pared.jpg", wallShader, wallTexture);
        Paint* p = paintPool.back();
        paintPool.pop_back();
        return p;
    }

    void returnModel(Model* m)
    {
        m->reset();
        modelPool.push_back(m);
    }
    
    void returnPaint(Paint* p)
    {
        p->reset();
        paintPool.push_back(p);
    }

    ~chunkLoader() {
        for (auto const& it : mundialMap) {
            returnPaint(it.second);  
        }
        for (auto const& it : dataMap) {
            returnModel(it.second);
        }
        mundialMap.clear();
        dataMap.clear();
        
        for (auto m : modelPool) {
            delete m;
        }
        modelPool.clear();
        for (auto p : paintPool) {
            delete p;
        }
        paintPool.clear();
    }

    void manageMemory(int cellx, int cellz, int radio)
    {
        for (auto it = mundialMap.begin(); it != mundialMap.end(); ) 
            {
                if (abs(it->first.x - cellx) > 3 || abs(it->first.z - cellz) > 3) 
                {
                    returnPaint(it->second);
                    if (dataMap.count(it->first)) 
                    {
                        returnModel(dataMap[it->first]);
                        dataMap.erase(it->first);
                    }
                        it = mundialMap.erase(it);
                } 
                else 
                {
                ++it;
                }
            }
    }

    void clearChunk(int cellx, int cellz, int radio)
    {
        static int lastCellX = -99999, lastCellZ = -99999;
                if (lastCellX != -99999) 
                {
                    if (cellx > lastCellX) 
                    {
                        int badX = lastCellX + radio;
                        for (auto it = mundialMap.begin(); it != mundialMap.end(); ) 
                        {
                            if (it->first.x == badX) 
                            {
                                returnPaint(it->second);  
                                if (dataMap.count(it->first)) 
                                {
                                    returnModel(dataMap[it->first]);  
                                    dataMap.erase(it->first);
                                }
                                it = mundialMap.erase(it);
                            } 
                            else 
                            {
                                ++it;
                            }
                        }
                    }

                    if (cellz > lastCellZ) 
                    {
                        int badZ = lastCellZ + radio;
                        for (auto it = mundialMap.begin(); it != mundialMap.end(); ) 
                        {
                            if (it->first.z == badZ) 
                            {
                                returnPaint(it->second); 
                                if (dataMap.count(it->first)) 
                                {
                                    returnModel(dataMap[it->first]);
                                    dataMap.erase(it->first);
                                }
                                it = mundialMap.erase(it);
                            } 
                            else 
                            {
                                ++it;
                            }
                        }
                    }
                }
                lastCellX = cellx; lastCellZ = cellz;        
    }

    void create(int cellx, int cellz, int radio, float escala)
    {
        std::map<Coordenada, Model*> tempModels;
        for (int x = cellx - radio; x <= cellx + radio; x++) {
                    for (int z = cellz - radio; z <= cellz + radio; z++) {
                        Coordenada c = {x, z};

                        if (dataMap.find(c) == dataMap.end())
                        {
                            Model* m = getModel();
                            m->processSeed(10 + x * 10 + z, x, z);  
                            dataMap[c] = m;
                        }
                        tempModels[c] = dataMap[c];
                    }
                }

                for (int x = cellx - radio; x <= cellx + radio; x++) {
                    for (int z = cellz - radio; z <= cellz + radio; z++) {
                        Coordenada coord = {x, z};
                        
                        if (mundialMap.find(coord) == mundialMap.end()) 
                        {
                            if (tempModels.count({x+1, z}) && tempModels[coord]->doorData.count(0)) {
                                DoorPoints p1 = tempModels[coord]->doorData[0], p2 = tempModels[{x+1, z}]->doorData[1];
                                //Sumamos la escala a los vertices de la puerta del vecino 
                                //Para trasladarla a su rejilla y sea local a la puerta origen
                                //sin esto, los pasillos se generarían dentro de un mismo cuarto
                                //joder sé que es difícil de visualizar, pero así funciona
                                p2.infIzq.x += escala;
                                p2.supIzq.x += escala;
                                p2.infDer.x += escala;
                                p2.supDer.x += escala;
                                tempModels[coord]->generateCorridor(p1, p2);
                            }
                            if (tempModels.count({x, z+1}) && tempModels[coord]->doorData.count(2)) 
                            {
                                DoorPoints p1 = tempModels[coord]->doorData[2], p2 = tempModels[{x, z+1}]->doorData[3];
                                p2.infIzq.z += escala;
                                p2.supIzq.z += escala; 
                                p2.infDer.z += escala; 
                                p2.supDer.z += escala;
                                tempModels[coord]->generateCorridor(p1, p2);
                            }

                                tempModels[coord]->updatePointers();
                                Paint* newRoom = getPaint();
                                newRoom->configBuffers(*tempModels[coord], true);
                                mundialMap[coord] = newRoom;
                        }
                        mundialMap[coord]->draw(glm::vec3(x * escala, 0.0f, z * escala));
                    }
                }        
    }

    void loadChunk(GLFWwindow *window, Camera &cam) {
        float escala = 45.0f;
        int cellx = (int)floor(cam.position.x / escala);
        int cellz = (int)floor(cam.position.z / escala);
        int radio = 2;

        //limpiar cuartos que no aparecen para no renderizarlos
        clearChunk(cellx, cellz, radio);
        
        //crear cuartos y pasillos
        create(cellx, cellz, radio, escala);
    
        //limpiar memoria
        static int cleanupCounter = 0;
        if (++cleanupCounter > 60) 
        {
            cleanupCounter = 0;
            manageMemory(cellx, cellz, 2);
            
        }
    }
};



#endif