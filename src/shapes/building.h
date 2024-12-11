// building.h
#ifndef BUILDING_H
#define BUILDING_H

#include <vector>
#include <glm/glm.hpp>
#include "cube.h"
#include "./utils/scenedata.h"

class Building {
public:
    Building() = default;
    void updateParams();
    std::vector<float> generateShape();

private:
    // Building parameters
    int m_numFloors = 1;
    int m_windowsPerFloor = 2;
    float m_floorHeight = 1.0f;
    float m_buildingWidth = 2.0f;
    float m_buildingDepth = 2.0f;
    int m_tessellation = 1;  // Corresponds to cube param1

    // Cube instance for generating building sections
    Cube m_cube;

    std::vector<float> m_vertexData;
    SceneMaterial sceneMaterial {glm::vec4(0.5f), glm::vec4(0.5f), glm::vec4(0.5f), 0.1};

    // Helper functions
    void generateBuildingSection(float width, float height, float depth, float xOffset, float yOffset, float zOffset);
    void addWindows();
    void insertVec3(glm::vec3 pos, glm::vec3 normal, glm::vec3 color);
    void transformVertices(std::vector<float>& vertices, float scale_x, float scale_y, float scale_z,
                           float translate_x, float translate_y, float translate_z);
};

#endif // BUILDING_H
