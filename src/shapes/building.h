#ifndef BUILDING_H
#define BUILDING_H

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 texCoord;
};

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

    // Vertex data storage
    std::vector<float> m_vertexData;

    // Helper functions for building construction
    void setVertexData();
    void makeFace(glm::vec3 topLeft, glm::vec3 topRight,
                  glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 normal);
    void addWindows();
    void insertVec3(glm::vec3 pos, glm::vec3 normal, glm::vec3 color);
};

#endif // BUILDING_H
