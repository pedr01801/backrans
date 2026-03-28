#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <glm/glm.hpp>
#include <map>

bool connectRight(int x, int z);
bool connectUp(int x, int z);

struct DoorPoints {
    glm::vec3 infIzq, supIzq, infDer, supDer;
};

class Model {
public:
    int w, h, d;
    std::map<int, DoorPoints> doorData;
    float* vertices;
    unsigned int* indices;
    size_t sizeVertices;
    size_t sizeIndices;
    int indicesAmount;

    std::vector<float> local_vertices;
    std::vector<unsigned int> local_indices;

    Model();
    void processSeed(int seed, int cellx, int cellz);
    void Model::generateCorridor(DoorPoints& p1, DoorPoints& p2);
    void addPlainSurface(int face, int w, int h, int d, unsigned int offset);
    void addDoorSurface(int face, int w, int h, int d, unsigned int offset);

    void updatePointers() {
        vertices = local_vertices.data();
        indices = local_indices.data();
        sizeVertices = local_vertices.size() * sizeof(float);
        sizeIndices = local_indices.size() * sizeof(unsigned int);
        indicesAmount = (int)local_indices.size();
    }

    void reset();
};

#endif