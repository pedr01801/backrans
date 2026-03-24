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

    // Mapeo consistente de puertas
    bool door[4];
    door[0] = connectRight(cellx, cellz);      // +X (Derecha)
    door[1] = connectRight(cellx - 1, cellz);  // -X (Izquierda)
    door[2] = connectUp(cellx, cellz);         // +Z (Frente)
    door[3] = connectUp(cellx, cellz - 1);     // -Z (Fondo)

    local_vertices.clear();
    local_indices.clear();

    // i: 0=+X, 1=-X, 2=+Z, 3=-Z, 4=Suelo, 5=Techo
    for (int i = 0; i < 6; i++) {
        unsigned int offset = local_vertices.size() / 5;

        if (i < 4 && door[i]) {
            // --- LÓGICA DE PUERTA (MARCO) ---
            float marco[] = {
                // Posiciones (Local)  // UVs
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
                float lx = marco[j*5];     // Local X
                float ly = marco[j*5+1];   // Local Y
                float worldX, worldY, worldZ, u, v;

                worldY = (ly + 0.5f) * h;
                v = worldY / 5.0f; // Tileado de textura vertical

                if (i == 0) { // Derecha (+X)
                    worldX = w/2.0f; worldZ = lx * d; u = (lx * d)/5.0f;
                } else if (i == 1) { // Izquierda (-X)
                    worldX = -w/2.0f; worldZ = -lx * d; u = (lx * d)/5.0f;
                } else if (i == 2) { // Frente (+Z)
                    worldX = -lx * w; worldZ = d/2.0f; u = (lx * w)/5.0f;
                } else { // Fondo (-Z)
                    worldX = lx * w; worldZ = -d/2.0f; u = (lx * w)/5.0f;
                }

                local_vertices.insert(local_vertices.end(), {worldX, worldY, worldZ, u, v});
            }
            unsigned int mInd[] = { 0,1,2, 2,3,0, 4,5,6, 6,7,4, 8,9,10, 10,11,8 };
            for(unsigned int idx : mInd) local_indices.push_back(idx + offset);

        } else {
            // --- LÓGICA DE PARED SÓLIDA / SUELO / TECHO ---
            for (int j = 0; j < 4; j++) {
                float lx = (j == 1 || j == 2) ? 0.5f : -0.5f;
                float ly = (j == 2 || j == 3) ? 0.5f : -0.5f;
                float worldX, worldY, worldZ, u, v;

                if (i == 0) { // Derecha (+X)
                    worldX = w/2.0f; worldY = (ly+0.5f)*h; worldZ = lx*d;
                    u = (lx*d)/5.0f; v = (worldY)/5.0f;
                } else if (i == 1) { // Izquierda (-X)
                    worldX = -w/2.0f; worldY = (ly+0.5f)*h; worldZ = -lx*d;
                    u = (lx*d)/5.0f; v = (worldY)/5.0f;
                } else if (i == 2) { // Frente (+Z)
                    worldX = -lx*w; worldY = (ly+0.5f)*h; worldZ = d/2.0f;
                    u = (lx*w)/5.0f; v = (worldY)/5.0f;
                } else if (i == 3) { // Fondo (-Z)
                    worldX = lx*w; worldY = (ly+0.5f)*h; worldZ = -d/2.0f;
                    u = (lx*w)/5.0f; v = (worldY)/5.0f;
                } else if (i == 4) { // Suelo
                    worldX = lx*w; worldY = 0.0f; worldZ = ly*d;
                    u = (lx*w)/5.0f; v = (ly*d)/5.0f;
                } else { // Techo
                    worldX = lx*w; worldY = h; worldZ = ly*d;
                    u = (lx*w)/5.0f; v = (ly*d)/5.0f;
                }
                local_vertices.insert(local_vertices.end(), {worldX, worldY, worldZ, u, v});
            }
            local_indices.insert(local_indices.end(), {offset+0, offset+1, offset+2, offset+2, offset+3, offset+0});
        }
    }

    // Actualizar punteros finales
    vertices = local_vertices.data();
    indices = local_indices.data();
    sizeVertices = local_vertices.size() * sizeof(float);
    sizeIndices = local_indices.size() * sizeof(unsigned int);
    indicesAmount = local_indices.size();
}