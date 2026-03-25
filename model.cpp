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

bool shouldForceRight(int x, int z) {
    bool r = connectRight(x, z);
    bool l = connectRight(x - 1, z);
    bool u = connectUp(x, z);
    bool d = connectUp(x, z - 1);
    int count = (r ? 1 : 0) + (l ? 1 : 0) + (u ? 1 : 0) + (d ? 1 : 0);
    // Si hay menos de 2 puertas y la derecha ta cerrada, la forzamos
    if (count < 2 && !r) return true;
    return false;
}

bool shouldForceUp(int x, int z) {
    bool r = connectRight(x, z);
    bool l = connectRight(x - 1, z);
    bool u = connectUp(x, z);
    bool d = connectUp(x, z - 1);
    int count = (r ? 1 : 0) + (l ? 1 : 0) + (u ? 1 : 0) + (d ? 1 : 0);

    // Simular el forzado de derecha para ver el count real
    if (count < 2 && !r) count++;
    
    // Si sigue habiendo menos de 2 y arriba ta cerrada, la forzamos
    if (count < 2 && !u) return true;
    return false;
}

void Model::processSeed(int n, int cellx, int cellz) {
    std::srand(n);
    Seeder sd;
    w = sd.generateSeed(30, 40);
    h = 10.0f;
    d = sd.generateSeed(30, 40);

    // 1. Determinar puertas (CONSISTENTE CON VECINOS)
    bool door[4];
    // Una puerta existe si el hash lo dice O si el cuarto correspondiente (este o el vecino) la fuerza
    // Door 0 (Right): Yo fuerzo derecha?
    door[0] = connectRight(cellx, cellz)     || shouldForceRight(cellx, cellz);
    
    // Door 1 (Left): El vecino de la izquierda (cellx-1) fuerza su derecha?
    door[1] = connectRight(cellx - 1, cellz) || shouldForceRight(cellx - 1, cellz);
    
    // Door 2 (Up): Yo fuerzo arriba?
    door[2] = connectUp(cellx, cellz)        || shouldForceUp(cellx, cellz);
    
    // Door 3 (Down): El vecino de abajo (cellz-1) fuerza su arriba?
    door[3] = connectUp(cellx, cellz - 1)    || shouldForceUp(cellx, cellz - 1);

    local_vertices.clear();
    local_indices.clear();
    doorData.clear();

    // 2. Generación de Paredes/Suelo/Techo
    for (int i = 0; i < 6; i++) {
        unsigned int offset = local_vertices.size() / 5;
        if (i < 4 && door[i]) {
            DoorPoints points;
            float doorH = 0.6f * h; 
            if (i == 0 || i == 1) { 
                float wx = (i == 0) ? w/2.0f : -w/2.0f;
                points.infIzq = glm::vec3(wx, 0.0f, -0.2f * d);
                points.supIzq = glm::vec3(wx, doorH, -0.2f * d);
                points.infDer = glm::vec3(wx, 0.0f,  0.2f * d);
                points.supDer = glm::vec3(wx, doorH,  0.2f * d);
            } else {
                float wz = (i == 2) ? d/2.0f : -d/2.0f;
                points.infIzq = glm::vec3(-0.2f * w, 0.0f, wz);
                points.supIzq = glm::vec3(-0.2f * w, doorH, wz);
                points.infDer = glm::vec3( 0.2f * w, 0.0f, wz);
                points.supDer = glm::vec3( 0.2f * w, doorH, wz);
            }
            doorData[i] = points;

            float marco[] = {
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -0.2f, -0.5f, 0.0f, 0.3f, 0.0f, -0.2f, 0.1f, 0.0f, 0.3f, 0.6f, -0.5f, 0.1f, 0.0f, 0.0f, 0.6f,
                 0.2f, -0.5f, 0.0f, 0.7f, 0.0f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.5f, 0.1f, 0.0f, 1.0f, 0.6f,  0.2f, 0.1f, 0.0f, 0.7f, 0.6f,
                -0.5f,  0.1f, 0.0f, 0.0f, 0.6f,  0.5f,  0.1f, 0.0f, 1.0f, 0.6f,  0.5f, 0.5f, 0.0f, 1.0f, 1.0f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
            };
            for (int j = 0; j < 12; j++) {
                float lx = marco[j*5], ly = marco[j*5+1], vx, vy, vz, u, v;
                vy = (ly + 0.5f) * h; v = vy / 5.0f;
                if (i == 0) { vx = w/2.0f; vz = lx*d; u = (lx*d)/5.0f; }
                else if (i == 1) { vx = -w/2.0f; vz = -lx*d; u = (lx*d)/5.0f; }
                else if (i == 2) { vx = -lx*w; vz = d/2.0f; u = (lx*w)/5.0f; }
                else { vx = lx*w; vz = -d/2.0f; u = (lx*w)/5.0f; }
                local_vertices.insert(local_vertices.end(), {vx, vy, vz, u, v});
            }
            unsigned int mInd[] = { 0,1,2, 2,3,0, 4,5,6, 6,7,4, 8,9,10, 10,11,8 };
            for(unsigned int idx : mInd) local_indices.push_back(idx + offset);
        } else {
            for (int j = 0; j < 4; j++) {
                float lx = (j == 1 || j == 2) ? 0.5f : -0.5f, ly = (j == 2 || j == 3) ? 0.5f : -0.5f, vx, vy, vz, u, v;
                if (i == 0) { vx = w/2.0f; vy = (ly+0.5f)*h; vz = lx*d; }
                else if (i == 1) { vx = -w/2.0f; vy = (ly+0.5f)*h; vz = -lx*d; }
                else if (i == 2) { vx = -lx*w; vy = (ly+0.5f)*h; vz = d/2.0f; }
                else if (i == 3) { vx = lx*w; vy = (ly+0.5f)*h; vz = -d/2.0f; }
                else if (i == 4) { vx = lx*w; vy = 0.0f; vz = ly*d; }
                else { vx = lx*w; vy = h; vz = ly*d; }
                u = (i < 4) ? (lx * ((i < 2) ? d : w)) / 5.0f : (lx * w) / 5.0f;
                v = (i < 4) ? vy / 5.0f : (ly * d) / 5.0f;
                local_vertices.insert(local_vertices.end(), {vx, vy, vz, u, v});
            }
            if (i == 5) local_indices.insert(local_indices.end(), {offset+0, offset+2, offset+1, offset+2, offset+0, offset+3});
            else local_indices.insert(local_indices.end(), {offset+0, offset+1, offset+2, offset+2, offset+3, offset+0});
        }
    }
    updatePointers();
}

void Model::generateCorridor(DoorPoints& p1, DoorPoints& p2) {
    unsigned int offset = local_vertices.size() / 5;
    bool isVert = std::abs(p1.infIzq.x - p1.infDer.x) > std::abs(p1.infIzq.z - p1.infDer.z);
    float aMax = std::max(isVert ? std::abs(p1.infIzq.x - p1.infDer.x) : std::abs(p1.infIzq.z - p1.infDer.z),
                          isVert ? std::abs(p2.infIzq.x - p2.infDer.x) : std::abs(p2.infIzq.z - p2.infDer.z));
    glm::vec3 c1 = (p1.infIzq + p1.infDer) * 0.5f, c2 = (p2.infIzq + p2.infDer) * 0.5f;
    glm::vec3 p1L = c1, p1R = c1, p2L = c2, p2R = c2;
    if (isVert) { p1L.x -= aMax*0.5f; p1R.x += aMax*0.5f; p2L.x -= aMax*0.5f; p2R.x += aMax*0.5f; }
    else { p1L.z -= aMax*0.5f; p1R.z += aMax*0.5f; p2L.z -= aMax*0.5f; p2R.z += aMax*0.5f; }
    float hC = p1.supIzq.y;
    local_vertices.insert(local_vertices.end(), { p1L.x, 0.0f, p1L.z, 0.0f, 0.0f, p1L.x, hC, p1L.z, 0.0f, 2.0f, 
                                                 p1R.x, 0.0f, p1R.z, 2.0f, 0.0f, p1R.x, hC, p1R.z, 2.0f, 2.0f,
                                                 p2L.x, 0.0f, p2L.z, 6.0f, 0.0f, p2L.x, hC, p2L.z, 6.0f, 2.0f,
                                                 p2R.x, 0.0f, p2R.z, 8.0f, 0.0f, p2R.x, hC, p2R.z, 8.0f, 2.0f });
    std::vector<unsigned int> ind = { 0,2,6, 6,4,0, 1,5,7, 7,3,1, 0,4,5, 5,1,0, 2,3,7, 7,6,2 };
    for(unsigned int i : ind) local_indices.push_back(i + offset);
    updatePointers();
}