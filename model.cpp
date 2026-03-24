#include <iostream>
#include "model.h"
#include "seeder.h"

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
            // CAPTURA DE VÉRTICES PARA EL PASILLO (Ingeniería de puntos internos)
            // Calculamos los 4 puntos internos que dan al hueco de la puerta
            DoorPoints points;
            float worldX, worldZ;
            
            // Altura del dintel según tu código (ly 0.1 local + 0.5 offset = 0.6 real)
            float doorH = 0.6f * h; 

            if (i == 0 || i == 1) { // Paredes en X
                worldX = (i == 0) ? w/2.0f : -w/2.0f;
                points.infIzq = glm::vec3(worldX, 0.0f, -0.2f * d);
                points.supIzq = glm::vec3(worldX, doorH, -0.2f * d);
                points.infDer = glm::vec3(worldX, 0.0f,  0.2f * d);
                points.supDer = glm::vec3(worldX, doorH,  0.2f * d);
            } else { // Paredes en Z
                worldZ = (i == 2) ? d/2.0f : -d/2.0f;
                points.infIzq = glm::vec3(-0.2f * w, 0.0f, worldZ);
                points.supIzq = glm::vec3(-0.2f * w, doorH, worldZ);
                points.infDer = glm::vec3( 0.2f * w, 0.0f, worldZ);
                points.supDer = glm::vec3( 0.2f * w, doorH, worldZ);
            }
            doorData[i] = points;

            // --- GEOMETRÍA DEL MARCO (Tu código de 12 vértices) ---
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
                if (i == 0) { vx = w/2.0f; vz = lx*d; u = (lx*d)/5.0f; }
                else if (i == 1) { vx = -w/2.0f; vz = -lx*d; u = (lx*d)/5.0f; }
                else if (i == 2) { vx = -lx*w; vz = d/2.0f; u = (lx*w)/5.0f; }
                else { vx = lx*w; vz = -d/2.0f; u = (lx*w)/5.0f; }
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
                if (i == 0) { vx = w/2.0f; vy = (ly+0.5f)*h; vz = lx*d; u = (lx*d)/5.0f; v = vy/5.0f; }
                else if (i == 1) { vx = -w/2.0f; vy = (ly+0.5f)*h; vz = -lx*d; u = (lx*d)/5.0f; v = vy/5.0f; }
                else if (i == 2) { vx = -lx*w; vy = (ly+0.5f)*h; vz = d/2.0f; u = (lx*w)/5.0f; v = vy/5.0f; }
                else if (i == 3) { vx = lx*w; vy = (ly+0.5f)*h; vz = -d/2.0f; u = (lx*w)/5.0f; v = vy/5.0f; }
                else if (i == 4) { vx = lx*w; vy = 0.0f; vz = ly*d; u = (lx*w)/5.0f; v = (ly*d)/5.0f; }
                else { vx = lx*w; vy = h; vz = ly*d; u = (lx*w)/5.0f; v = (ly*d)/5.0f; }
                local_vertices.insert(local_vertices.end(), {vx, vy, vz, u, v});
            }
            local_indices.insert(local_indices.end(), {offset+0, offset+1, offset+2, offset+2, offset+3, offset+0});
        }
    }
    // ... actualización de punteros final (igual que antes) ...
    vertices = local_vertices.data();
    indices = local_indices.data();
    sizeVertices = local_vertices.size() * sizeof(float);
    sizeIndices = local_indices.size() * sizeof(unsigned int);
    indicesAmount = local_indices.size();
}

void Model::generateCorridor(DoorPoints& p1, DoorPoints& p2) {
    unsigned int offset = local_vertices.size() / 5;

    // 1. DETERMINAR DIRECCIÓN Y PROYECCIÓN
    // Si la diferencia en X es muy pequeña, el pasillo va en el eje Z (Norte/Sur)
    // Si la diferencia en Z es muy pequeña, el pasillo va en el eje X (Este/Oeste)
    bool isVertical = std::abs(p1.infIzq.x - p1.infDer.x) > std::abs(p1.infIzq.z - p1.infDer.z);
    
    // 2. NORMALIZAR ANCHO (Evitar el efecto embudo)
    // Calculamos el ancho de ambas puertas y nos quedamos con el máximo
    float ancho1 = isVertical ? std::abs(p1.infIzq.x - p1.infDer.x) : std::abs(p1.infIzq.z - p1.infDer.z);
    float ancho2 = isVertical ? std::abs(p2.infIzq.x - p2.infDer.x) : std::abs(p2.infIzq.z - p2.infDer.z);
    float anchoMax = std::max(ancho1, ancho2);

    // Re-calculamos puntos de p1 (inicio) basados en el ancho máximo centrado
    glm::vec3 centroP1 = (p1.infIzq + p1.infDer) * 0.5f;
    glm::vec3 p1_L = centroP1;
    glm::vec3 p1_R = centroP1;

    // Re-calculamos puntos de p2 (fin) basados en el mismo ancho máximo
    glm::vec3 centroP2 = (p2.infIzq + p2.infDer) * 0.5f;
    glm::vec3 p2_L = centroP2;
    glm::vec3 p2_R = centroP2;

    if (isVertical) {
        p1_L.x -= anchoMax * 0.5f; p1_R.x += anchoMax * 0.5f;
        p2_L.x -= anchoMax * 0.5f; p2_R.x += anchoMax * 0.5f;
    } else {
        p1_L.z -= anchoMax * 0.5f; p1_R.z += anchoMax * 0.5f;
        p2_L.z -= anchoMax * 0.5f; p2_R.z += anchoMax * 0.5f;
    }

    float h = p1.supIzq.y; // Usamos la altura de la puerta

    // 3. INSERTAR VÉRTICES (4 del inicio, 4 del fin proyectado)
    // Inicio (Cuarto actual)
    local_vertices.insert(local_vertices.end(), { p1_L.x, 0.0f, p1_L.z, 0.0f, 0.0f }); // 0: infIzq
    local_vertices.insert(local_vertices.end(), { p1_L.x, h,    p1_L.z, 0.0f, 2.0f }); // 1: supIzq
    local_vertices.insert(local_vertices.end(), { p1_R.x, 0.0f, p1_R.z, 2.0f, 0.0f }); // 2: infDer
    local_vertices.insert(local_vertices.end(), { p1_R.x, h,    p1_R.z, 2.0f, 2.0f }); // 3: supDer

    // Fin (Proyectado al vecino)
    local_vertices.insert(local_vertices.end(), { p2_L.x, 0.0f, p2_L.z, 5.0f, 0.0f }); // 4
    local_vertices.insert(local_vertices.end(), { p2_L.x, h,    p2_L.z, 5.0f, 2.0f }); // 5
    local_vertices.insert(local_vertices.end(), { p2_R.x, 0.0f, p2_R.z, 7.0f, 0.0f }); // 6
    local_vertices.insert(local_vertices.end(), { p2_R.x, h,    p2_R.z, 7.0f, 2.0f }); // 7

    // 4. ÍNDICES (Suelo y Paredes)
    std::vector<unsigned int> corridorIndices = {
        0, 2, 6, 6, 4, 0, // Suelo
        0, 4, 5, 5, 1, 0, // Pared Izquierda
        2, 3, 7, 7, 6, 2  // Pared Derecha
    };

    for(unsigned int idx : corridorIndices) local_indices.push_back(idx + offset);
}