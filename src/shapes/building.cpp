#include "building.h"
#include <iostream>
#include "../settings.h"

void Building::updateParams() {
    // random building parameters
    m_numFloors = 3 + rand() % 8;  // 3 to 10 floors
    m_windowsPerFloor = 2 + rand() % 3;  // 2 to 4 windows
    m_floorHeight = 0.8f + (rand() % 100) / 100.0f * 0.8f;
    m_buildingWidth = 1.0f + (rand() % (int)settings.buildingMaxWidth) / 2.f;
    m_buildingDepth = 1.0f + (rand() % (int)settings.buildingMaxWidth) / 2.f;

    // random tessellation for variety in building faces
    m_tessellation = 1 + rand() % 8;

    generateShape();
}

std::vector<float> Building::generateShape() {
    m_vertexData.clear();

    // Generate main building structure
    generateBuildingSection(m_buildingWidth, m_numFloors * m_floorHeight, m_buildingDepth, 0, 0, 0);

    // Add windows
    if (m_windowsPerFloor > 0) {
        addWindows();
    }

    return m_vertexData;
}

void Building::generateBuildingSection(float width, float height, float depth,
                                       float xOffset, float yOffset, float zOffset) {
    // Update cube tessellation
    m_cube.updateParams(m_tessellation);

    // Get base cube vertices
    std::vector<float> cubeVertices = m_cube.generateShape();

    // Transform vertices to building dimensions and position
    transformVertices(cubeVertices, width, height, depth, xOffset, yOffset + height/2, zOffset);

    // Add wall color to vertices
    for (size_t i = 0; i < cubeVertices.size(); i += 6) {  // pos + normal = 6 values
        glm::vec3 wallColor = settings.wallColor;
        m_vertexData.push_back(cubeVertices[i]);     // position.x
        m_vertexData.push_back(cubeVertices[i+1]);   // position.y
        m_vertexData.push_back(cubeVertices[i+2]);   // position.z
        m_vertexData.push_back(cubeVertices[i+3]);   // normal.x
        m_vertexData.push_back(cubeVertices[i+4]);   // normal.y
        m_vertexData.push_back(cubeVertices[i+5]);   // normal.z
        m_vertexData.push_back(wallColor.r);         // color.r
        m_vertexData.push_back(wallColor.g);         // color.g
        m_vertexData.push_back(wallColor.b);         // color.b
    }
}

void Building::transformVertices(std::vector<float>& vertices, float scale_x, float scale_y, float scale_z,
                                 float translate_x, float translate_y, float translate_z) {
    for (size_t i = 0; i < vertices.size(); i += 6) {  // 6 values per vertex (pos + normal)
        // Scale
        vertices[i] *= scale_x;
        vertices[i + 1] *= scale_y;
        vertices[i + 2] *= scale_z;

        // Translate
        vertices[i] += translate_x;
        vertices[i + 1] += translate_y;
        vertices[i + 2] += translate_z;
    }
}

void Building::insertVec3(glm::vec3 pos, glm::vec3 normal, glm::vec3 color) {
    normal = glm::normalize(normal);

    // position
    m_vertexData.push_back(pos.x);
    m_vertexData.push_back(pos.y);
    m_vertexData.push_back(pos.z);

    // normal
    m_vertexData.push_back(normal.x);
    m_vertexData.push_back(normal.y);
    m_vertexData.push_back(normal.z);

    // color
    m_vertexData.push_back(color.r);
    m_vertexData.push_back(color.g);
    m_vertexData.push_back(color.b);
}

void Building::addWindows() {
    float windowWidth = settings.windowWidth;
    float windowHeight = settings.windowHeight;
    glm::vec3 regularWindowColor = settings.windowColor;
    glm::vec3 litWindowColor = glm::vec3(2.0f, 1.9f, 1.4f);

    // Space windows evenly across faces
    float spacing = m_buildingWidth / (m_windowsPerFloor + 1);
    float zSpacing = m_buildingDepth / (m_windowsPerFloor + 1);

    // Generate windows for each floor
    for (int floor = 0; floor < m_numFloors; floor++) {
        float floorBase = floor * m_floorHeight;

        for (int window = 0; window < m_windowsPerFloor; window++) {
            float windowX = -m_buildingWidth/2 + spacing * (window + 1);
            float windowY = floorBase + 0.25f;

            // Determine if window should be lit
            bool isLit = (rand() % 100) < 25;  // 25% chance of being lit
            glm::vec3 windowColor = isLit ? litWindowColor : regularWindowColor;

            // Front face windows
            glm::vec3 frontNormal(0, 0, 1);
            insertVec3(glm::vec3(windowX - windowWidth/2, windowY + windowHeight, m_buildingDepth/2 + 0.01f), frontNormal, windowColor);
            insertVec3(glm::vec3(windowX - windowWidth/2, windowY, m_buildingDepth/2 + 0.01f), frontNormal, windowColor);
            insertVec3(glm::vec3(windowX + windowWidth/2, windowY + windowHeight, m_buildingDepth/2 + 0.01f), frontNormal, windowColor);
            insertVec3(glm::vec3(windowX + windowWidth/2, windowY + windowHeight, m_buildingDepth/2 + 0.01f), frontNormal, windowColor);
            insertVec3(glm::vec3(windowX - windowWidth/2, windowY, m_buildingDepth/2 + 0.01f), frontNormal, windowColor);
            insertVec3(glm::vec3(windowX + windowWidth/2, windowY, m_buildingDepth/2 + 0.01f), frontNormal, windowColor);

            // Back face windows
            glm::vec3 backNormal(0, 0, -1);
            insertVec3(glm::vec3(windowX + windowWidth/2, windowY + windowHeight, -m_buildingDepth/2 - 0.01f), backNormal, windowColor);
            insertVec3(glm::vec3(windowX + windowWidth/2, windowY, -m_buildingDepth/2 - 0.01f), backNormal, windowColor);
            insertVec3(glm::vec3(windowX - windowWidth/2, windowY + windowHeight, -m_buildingDepth/2 - 0.01f), backNormal, windowColor);
            insertVec3(glm::vec3(windowX - windowWidth/2, windowY + windowHeight, -m_buildingDepth/2 - 0.01f), backNormal, windowColor);
            insertVec3(glm::vec3(windowX + windowWidth/2, windowY, -m_buildingDepth/2 - 0.01f), backNormal, windowColor);
            insertVec3(glm::vec3(windowX - windowWidth/2, windowY, -m_buildingDepth/2 - 0.01f), backNormal, windowColor);

            // Left face windows
            glm::vec3 leftNormal(-1, 0, 0);
            float zPos = -m_buildingDepth/2 + zSpacing * (window + 1);
            insertVec3(glm::vec3(-m_buildingWidth/2 - 0.01f, windowY + windowHeight, zPos), leftNormal, windowColor);
            insertVec3(glm::vec3(-m_buildingWidth/2 - 0.01f, windowY, zPos), leftNormal, windowColor);
            insertVec3(glm::vec3(-m_buildingWidth/2 - 0.01f, windowY + windowHeight, zPos + windowWidth), leftNormal, windowColor);
            insertVec3(glm::vec3(-m_buildingWidth/2 - 0.01f, windowY + windowHeight, zPos + windowWidth), leftNormal, windowColor);
            insertVec3(glm::vec3(-m_buildingWidth/2 - 0.01f, windowY, zPos), leftNormal, windowColor);
            insertVec3(glm::vec3(-m_buildingWidth/2 - 0.01f, windowY, zPos + windowWidth), leftNormal, windowColor);

            // Right face windows
            glm::vec3 rightNormal(1, 0, 0);
            insertVec3(glm::vec3(m_buildingWidth/2 + 0.01f, windowY + windowHeight, zPos + windowWidth), rightNormal, windowColor);
            insertVec3(glm::vec3(m_buildingWidth/2 + 0.01f, windowY, zPos + windowWidth), rightNormal, windowColor);
            insertVec3(glm::vec3(m_buildingWidth/2 + 0.01f, windowY + windowHeight, zPos), rightNormal, windowColor);
            insertVec3(glm::vec3(m_buildingWidth/2 + 0.01f, windowY + windowHeight, zPos), rightNormal, windowColor);
            insertVec3(glm::vec3(m_buildingWidth/2 + 0.01f, windowY, zPos + windowWidth), rightNormal, windowColor);
            insertVec3(glm::vec3(m_buildingWidth/2 + 0.01f, windowY, zPos), rightNormal, windowColor);
        }
    }
}
