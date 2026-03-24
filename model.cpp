#include <iostream>
#include "model.h"
#include "seeder.h"
#include <algorithm>

Model::Model() {
    vertices = nullptr;
    indices = nullptr;
}

static unsigned int hash3(int x, int z, int salt){
    unsigned int h = 2166136261u;
    h = (h ^ (unsigned int)x)    * 16777619u;
    h = (h ^ (unsigned int)z)    * 16777619u;
    h = (h ^ (unsigned int)salt) * 16777619u;
    return h;
}

bool connectRight(int x, int z){
    return (hash3(x, z, 0) % 100) < 40;
}

bool connectUp(int x, int z){
    return (hash3(x, z, 1) % 100) < 40;
}

void Model::processSeed(int n, int cellx, int cellz) {
    std::srand(n);
    Seeder sd;
    w = sd.generateSeed(30, 40);
    h = 10.0f;
    d = sd.generateSeed(30, 40);

    bool door[4];
    door[0] = connectRight(cellx, cellz);      // +X
    door[1] = connectRight(cellx - 1, cellz);  // -X
    door[2] = connectUp(cellx, cellz);         // +Z
    door[3] = connectUp(cellx, cellz - 1);     // -Z

    local_vertices.clear();
    local_indices.clear();
    doorData.clear();

    for (int i = 0; i < 6; i++) {
        unsigned int offset = local_vertices.size() / 5;

        if (i < 4 && door[i]) {
            // --- GEOMETRÍA CON PUERTA ---
            DoorPoints points;
            float doorH = 0.6f * h; 
            
            // Definición de puntos para DoorData (usado por los pasillos)
            if (i == 0 || i == 1) { // Paredes en X
                float worldX = (i == 0) ? w/2.0f : -w/2.0f;
                points.infIzq = glm::vec3(worldX, 0.0f, -0.2f * d);
                points.supIzq = glm::vec3(worldX, doorH, -0.2f * d);
                points.infDer = glm::vec3(worldX, 0.0f,  0.2f * d);
                points.supDer = glm::vec3(worldX, doorH,  0.2f * d);
            } else { // Paredes en Z
                float worldZ = (i == 2) ? d/2.0f : -d/2.0f;
                points.infIzq = glm::vec3(-0.2f * w, 0.0f, worldZ);
                points.supIzq = glm::vec3(-0.2f * w, doorH, worldZ);
                points.infDer = glm::vec3( 0.2f * w, 0.0f, worldZ);
                points.supDer = glm::vec3( 0.2f * w, doorH, worldZ);
            }
            doorData[i] = points;

            // Vértices del marco (12 vértices para dejar el hueco)
            float marco[] = {
                -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,
                -0.2f, -0.5f, 0.0f,    0.3f, 0.0f,
                -0.2f,  0.1f, 0.0f,    0.3f, 0.6f,
                -0.5f,  0.1f, 0.0f,    0.0f, 0.6f, // Poste Izq
                 0.2f, -0.5f, 0.0f,    0.7f, 0.0f,  
                 0.5f, -0.5f, 0.0f,    1.0f, 0.0f,  
                 0.5f,  0.1f, 0.0f,    1.0f, 0.6f,  
                 0.2f,  0.1f, 0.0f,    0.7f, 0.6f, // Poste Der
                -0.5f,  0.1f, 0.0f,    0.0f, 0.6f,  
                 0.5f,  0.1f, 0.0f,    1.0f, 0.6f,  
                 0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 
                -0.5f,  0.5f, 0.0f,    0.0f, 1.0f  // Dintel
            };

            for (int j = 0; j < 12; j++) {
                float lx = marco[j*5]; float ly = marco[j*5+1];
                float vx, vy, vz, u, v;
                vy = (ly + 0.5f) * h;
                v = vy / 5.0f;
                if (i == 0)      { vx =  w/2.0f; vz =  lx*d; u = (lx*d)/5.0f; }
                else if (i == 1) { vx = -w/2.0f; vz = -lx*d; u = (lx*d)/5.0f; }
                else if (i == 2) { vx = -lx*w;   vz =  d/2.0f; u = (lx*w)/5.0f; }
                else             { vx =  lx*w;   vz = -d/2.0f; u = (lx*w)/5.0f; }
                local_vertices.insert(local_vertices.end(), {vx, vy, vz, u, v});
            }
            unsigned int mInd[] = { 0,1,2, 2,3,0, 4,5,6, 6,7,4, 8,9,10, 10,11,8 };
            for(unsigned int idx : mInd) local_indices.push_back(idx + offset);

        } else {
            // --- PARED SÓLIDA / SUELO / TECHO ---
            for (int j = 0; j < 4; j++) {
                float lx = (j == 1 || j == 2) ? 0.5f : -0.5f;
                float ly = (j == 2 || j == 3) ? 0.5f : -0.5f;
                float vx, vy, vz, u, v;

                if (i == 0)      { vx =  w/2.0f; vy = (ly+0.5f)*h; vz =  lx*d; }
                else if (i == 1) { vx = -w/2.0f; vy = (ly+0.5f)*h; vz = -lx*d; }
                else if (i == 2) { vx = -lx*w;   vy = (ly+0.5f)*h; vz =  d/2.0f; }
                else if (i == 3) { vx =  lx*w;   vy = (ly+0.5f)*h; vz = -d/2.0f; }
                else if (i == 4) { vx =  lx*w;   vy = 0.0f;        vz =  ly*d; } // SUELO
                else             { vx =  lx*w;   vy = h;           vz =  ly*d; } // TECHO

                u = (i < 4) ? (lx* ( (i<2)?d:w ) )/5.0f : (lx*w)/5.0f;
                v = (i < 4) ? vy/5.0f : (ly*d)/5.0f;
                
                local_vertices.insert(local_vertices.end(), {vx, vy, vz, u, v});
            }

            // Invertimos el orden de índices para el TECHO (i=5) para que mire hacia abajo
            if (i == 5) {
                local_indices.insert(local_indices.end(), {offset+0, offset+2, offset+1, offset+2, offset+0, offset+3});
            } else {
                local_indices.insert(local_indices.end(), {offset+0, offset+1, offset+2, offset+2, offset+3, offset+0});
            }
        }
    }
    
    // Sincronización final de punteros
    vertices = local_vertices.data();
    indices = local_indices.data();
    sizeVertices = local_vertices.size() * sizeof(float);
    sizeIndices = local_indices.size() * sizeof(unsigned int);
    indicesAmount = local_indices.size();
}

void Model::generateCorridor(DoorPoints& p1, DoorPoints& p2) {
    unsigned int offset = local_vertices.size() / 5;

    // 1. Detectar orientación basándose en la apertura de la puerta
    bool isVertical = std::abs(p1.infIzq.x - p1.infDer.x) > std::abs(p1.infIzq.z - p1.infDer.z);
    
    // 2. Normalizar Ancho (Usar el mayor de los dos para evitar embudos)
    float ancho1 = isVertical ? std::abs(p1.infIzq.x - p1.infDer.x) : std::abs(p1.infIzq.z - p1.infDer.z);
    float ancho2 = isVertical ? std::abs(p2.infIzq.x - p2.infDer.x) : std::abs(p2.infIzq.z - p2.infDer.z);
    float anchoMax = std::max(ancho1, ancho2);

    glm::vec3 centroP1 = (p1.infIzq + p1.infDer) * 0.5f;
    glm::vec3 centroP2 = (p2.infIzq + p2.infDer) * 0.5f;

    glm::vec3 p1_L = centroP1, p1_R = centroP1;
    glm::vec3 p2_L = centroP2, p2_R = centroP2;

    if (isVertical) {
        p1_L.x -= anchoMax * 0.5f; p1_R.x += anchoMax * 0.5f;
        p2_L.x -= anchoMax * 0.5f; p2_R.x += anchoMax * 0.5f;
    } else {
        p1_L.z -= anchoMax * 0.5f; p1_R.z += anchoMax * 0.5f;
        p2_L.z -= anchoMax * 0.5f; p2_R.z += anchoMax * 0.5f;
    }

    float hCorridor = p1.supIzq.y;

    // 3. Vértices (Suelo y Techo del pasillo incluidos)
    // Inicio pasillo (0-3)
    local_vertices.insert(local_vertices.end(), { p1_L.x, 0.0f,      p1_L.z, 0.0f, 0.0f }); // 0
    local_vertices.insert(local_vertices.end(), { p1_L.x, hCorridor, p1_L.z, 0.0f, 2.0f }); // 1
    local_vertices.insert(local_vertices.end(), { p1_R.x, 0.0f,      p1_R.z, 2.0f, 0.0f }); // 2
    local_vertices.insert(local_vertices.end(), { p1_R.x, hCorridor, p1_R.z, 2.0f, 2.0f }); // 3

    // Fin pasillo (proyectado 4-7)
    local_vertices.insert(local_vertices.end(), { p2_L.x, 0.0f,      p2_L.z, 6.0f, 0.0f }); // 4
    local_vertices.insert(local_vertices.end(), { p2_L.x, hCorridor, p2_L.z, 6.0f, 2.0f }); // 5
    local_vertices.insert(local_vertices.end(), { p2_R.x, 0.0f,      p2_R.z, 8.0f, 0.0f }); // 6
    local_vertices.insert(local_vertices.end(), { p2_R.x, hCorridor, p2_R.z, 8.0f, 2.0f }); // 7

    // 4. Índices (Suelo, Paredes y Techo del pasillo)
    std::vector<unsigned int> ind = {
        0, 2, 6, 6, 4, 0, // Suelo
        1, 5, 7, 7, 3, 1, // Techo (mirando abajo)
        0, 4, 5, 5, 1, 0, // Pared Izq
        2, 3, 7, 7, 6, 2  // Pared Der
    };

    for(unsigned int i : ind) local_indices.push_back(i + offset);
}