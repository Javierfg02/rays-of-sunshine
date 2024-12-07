#include <GL/glew.h>
#include <glm/glm.hpp>

#include "./shapes/building.h"
#include "utils/sceneparser.h"
#include "utils/scenedata.h"

#ifndef BUILDINGGENERATOR_H
#define BUILDINGGENERATOR_H

class BuildingGenerator {

public:

    BuildingGenerator();
    ~BuildingGenerator();

    struct GridCell {
        int row;
        int col;
        bool isRoad;
        Building* building; // building related to that cell (if any)
    };

    struct Grid {
        std::vector<GridCell> grid; // 1D array of grids
    };

    std::vector<float> generateBuildings();
    void generateGrid();

    void renderBuildings();
    void initializeBuildings();
    GLuint getShader();

private:
    GLuint m_shader;
    GLbuffers buildingBuffers;
    Grid m_grid;
    std::vector<float> m_buildingData;

};

#endif // BUILDINGGENERATOR_H
