#include <GL/glew.h>
#include <glm/glm.hpp>

#include "./shapes/building.h"
#include "utils/sceneparser.h"
#include "utils/scenedata.h"

#ifndef BUILDINGGENERATOR_H
#define BUILDINGGENERATOR_H

class BuildingGenerator {

public:

    BuildingGenerator(float citySize = 150.f);
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
    GLuint getShader();
    glm::vec3 getRandomRoadPosition();
    std::vector<float> getRoadData();

private:
    float citySize;
    std::vector<GridCell> m_grid;
    GLbuffers m_roadBuffers;

    glm::mat4 getRandomRotatedTransform(float x, float z);
    std::vector<float> generateRoad();
    std::vector<float> m_roadVertexData;
    void addQuad(glm::vec3 bl, glm::vec3 br, glm::vec3 tl, glm::vec3 tr,
                 glm::vec3 normal, glm::vec3 color);

};

#endif // BUILDINGGENERATOR_H
