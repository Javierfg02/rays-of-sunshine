#include <GL/glew.h>
#include <glm/glm.hpp>

#include "./shapes/building.h"
#include "utils/sceneparser.h"
#include "utils/scenedata.h"

#ifndef BUILDINGGENERATOR_H
#define BUILDINGGENERATOR_H

class BuildingGenerator {

public:

    BuildingGenerator(float citySize = 100.f);
    ~BuildingGenerator();

    struct GridCell {
        int width;
        int height;
        int row;
        int col;
        bool isRoad;
        Building* building; // building related to that cell (if any)
        bool isInit = false;
    };

    std::vector<float> initializeBuildings();
    void generateGrid();
    void renderBuildings();
    GLuint getShader();

private:
    float citySize;
    GLuint m_shader;
    GLbuffers buildingBuffers;
    std::vector<GridCell> m_grid;
};

#endif // BUILDINGGENERATOR_H
