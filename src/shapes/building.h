#ifndef BUILDING_H
#define BUILDING_H

#include <vector>
#include <glm/glm.hpp>

#include "./utils/scenedata.h"


class Building {
public:
    Building() = default;

    // Update building parameters
    void updateParams();

    // Generate vertex data for the building
    std::vector<float> generateShape();

private:
    // Building parameters
    int m_numFloors = 1;
    int m_windowsPerFloor = 2;
    float m_floorHeight = 1.0f;
    float m_buildingWidth = 2.0f;
    float m_buildingDepth = 2.0f;

    // material
    SceneMaterial sceneMaterial {glm::vec4(0.5f), glm::vec4(0.5f), glm::vec4(0.5f), 0.1};

    // Vertex data storage
    std::vector<float> m_vertexData;

    // Helper functions for building construction
    void setVertexData();
    void makeFace(glm::vec3 topLeft, glm::vec3 topRight,
                  glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 normal);
    void insertVec3(glm::vec3 pos, glm::vec3 normal, glm::vec3 color);

    // windows
    bool shouldWindowBeLit() {
        return (rand() % 100) < 30; // 30% chance for a window to be lit
    }
    glm::vec3 getLitWindowColor() {
        // bright warm yellow for lit windows
        return glm::vec3(1.0f, 0.95f, 0.8f);
    }
    void addWindows();
};

#endif // BUILDING_H
