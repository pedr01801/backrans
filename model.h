#ifndef MODEL_H
#define MODEL_H

#include <vector>

bool connectRight(int x, int z);
bool connectUp(int x, int z);


class Model {
public:
    int w, h, d;
    float* vertices;
    unsigned int* indices;
    size_t sizeVertices;
    size_t sizeIndices;
    int indicesAmount;

    std::vector<float> local_vertices;
    std::vector<unsigned int> local_indices;

    Model();
    void processSeed(int seed, int cellx, int cellz);

};

#endif