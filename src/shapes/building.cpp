#include "building.h"
#include <iostream>
#include "../settings.h"

void Building::updateParams() {
    // random building parameters
    int numFloors = 3 + rand() % 8;  // 3 to 10 floors
    int windowsPerFloor = 2 + rand() % 3;  // 2 to 4 windows
    float floorHeight = 0.8f + (rand() % 100) / 100.0f * 0.8f;
    float buildingWidth = 1.0f + (rand() % (int) settings.buildingMaxWidth) / settings.buildingMaxWidth; // width is 0 - 100
    float buildingDepth = 1.0f + (rand() % (int) settings.buildingMaxWidth) / settings.buildingMaxWidth;; // depth is 0 - 100
    m_numFloors = numFloors;
    m_windowsPerFloor = windowsPerFloor;
    m_floorHeight = floorHeight;
    m_buildingWidth = buildingWidth;
    m_buildingDepth = buildingDepth;

    // TODO: add a randomized (or pseudo randomized) texture to each building - that is how we will make them look more real

    // generate the building
    setVertexData();
}

std::vector<float> Building::generateShape() {
    return m_vertexData;
}

void Building::insertVec3WithColor(std::vector<float> &data, glm::vec3 pos, glm::vec3 color) {
    // position
    data.push_back(pos.x);
    data.push_back(pos.y);
    data.push_back(pos.z);
    // color
    data.push_back(color.r);
    data.push_back(color.g);
    data.push_back(color.b);
}

void Building::setVertexData() {
    m_vertexData.clear();

    float width = m_buildingWidth;
    float depth = m_buildingDepth;
    float totalHeight = m_numFloors * m_floorHeight;

    // front face (facing towards camera, z positive)
    glm::vec3 frontTopLeft(-width/2, totalHeight, depth/2);
    glm::vec3 frontTopRight(width/2, totalHeight, depth/2);
    glm::vec3 frontBottomLeft(-width/2, 0, depth/2);
    glm::vec3 frontBottomRight(width/2, 0, depth/2);
    makeFace(frontTopLeft, frontTopRight, frontBottomLeft, frontBottomRight);

    // back face (facing away from camera, z negative)
    glm::vec3 backTopLeft(-width/2, totalHeight, -depth/2);
    glm::vec3 backTopRight(width/2, totalHeight, -depth/2);
    glm::vec3 backBottomLeft(-width/2, 0, -depth/2);
    glm::vec3 backBottomRight(width/2, 0, -depth/2);
    // Note: back face needs reverse winding order
    makeFace(backTopRight, backTopLeft, backBottomRight, backBottomLeft);

    // right face (x positive)
    glm::vec3 rightTopLeft(width/2, totalHeight, -depth/2);
    glm::vec3 rightTopRight(width/2, totalHeight, depth/2);
    glm::vec3 rightBottomLeft(width/2, 0, -depth/2);
    glm::vec3 rightBottomRight(width/2, 0, depth/2);
    makeFace(rightTopRight, rightTopLeft, rightBottomRight, rightBottomLeft);

    // left face (x negative)
    glm::vec3 leftTopLeft(-width/2, totalHeight, -depth/2);
    glm::vec3 leftTopRight(-width/2, totalHeight, depth/2);
    glm::vec3 leftBottomLeft(-width/2, 0, -depth/2);
    glm::vec3 leftBottomRight(-width/2, 0, depth/2);
    makeFace(leftTopLeft, leftTopRight, leftBottomLeft, leftBottomRight);

    // top face
    makeFace(frontTopRight, frontTopLeft, backTopRight, backTopLeft);

    // bottom face
    makeFace(frontBottomLeft, frontBottomRight, backBottomLeft, backBottomRight);

    // Add windows after basic structure
    if (m_windowsPerFloor > 0) {
        addWindows();
    }
}

void Building::makeFace(glm::vec3 topLeft, glm::vec3 topRight,
                        glm::vec3 bottomLeft, glm::vec3 bottomRight) {
    glm::vec3 wallColor = settings.wallColor; // Light gray for walls

    // Draw face using consistent counter-clockwise winding order
    // First triangle
    insertVec3WithColor(m_vertexData, bottomLeft, wallColor);
    insertVec3WithColor(m_vertexData, bottomRight, wallColor);
    insertVec3WithColor(m_vertexData, topRight, wallColor);

    // Second triangle
    insertVec3WithColor(m_vertexData, bottomLeft, wallColor);
    insertVec3WithColor(m_vertexData, topRight, wallColor);
    insertVec3WithColor(m_vertexData, topLeft, wallColor);
}


void Building::addWindows() {
    float windowWidth = settings.windowWidth;
    float windowHeight = settings.windowHeight;
    glm::vec3 windowColor = settings.windowColor;

    // Space windows evenly across the front face
    float spacing = m_buildingWidth / (m_windowsPerFloor + 1);

    // adding the windows to front back and multiple for each floor
    for (int floor = 0; floor < m_numFloors; floor++) {
        float floorBase = floor * m_floorHeight;

        for (int window = 0; window < m_windowsPerFloor; window++) {
            float windowX = -m_buildingWidth/2 + spacing * (window + 1);
            float windowY = floorBase + 0.5f; // Position windows halfway up each floor

            // Front face windows
            glm::vec3 frontTopLeft(windowX - windowWidth/2, windowY + windowHeight, m_buildingDepth/2 + 0.01f);
            glm::vec3 frontTopRight(windowX + windowWidth/2, windowY + windowHeight, m_buildingDepth/2 + 0.01f);
            glm::vec3 frontBottomLeft(windowX - windowWidth/2, windowY, m_buildingDepth/2 + 0.01f);
            glm::vec3 frontBottomRight(windowX + windowWidth/2, windowY, m_buildingDepth/2 + 0.01f);

            // front face windows
            insertVec3WithColor(m_vertexData, frontTopLeft, windowColor);
            insertVec3WithColor(m_vertexData, frontBottomLeft, windowColor);
            insertVec3WithColor(m_vertexData, frontTopRight, windowColor);
            insertVec3WithColor(m_vertexData, frontTopRight, windowColor);
            insertVec3WithColor(m_vertexData, frontBottomLeft, windowColor);
            insertVec3WithColor(m_vertexData, frontBottomRight, windowColor);

            // back face windows
            glm::vec3 backTopLeft(windowX - windowWidth/2, windowY + windowHeight, -m_buildingDepth/2 - 0.01f);
            glm::vec3 backTopRight(windowX + windowWidth/2, windowY + windowHeight, -m_buildingDepth/2 - 0.01f);
            glm::vec3 backBottomLeft(windowX - windowWidth/2, windowY, -m_buildingDepth/2 - 0.01f);
            glm::vec3 backBottomRight(windowX + windowWidth/2, windowY, -m_buildingDepth/2 - 0.01f);


            insertVec3WithColor(m_vertexData, backTopRight, windowColor);
            insertVec3WithColor(m_vertexData, backBottomRight, windowColor);
            insertVec3WithColor(m_vertexData, backTopLeft, windowColor);
            insertVec3WithColor(m_vertexData, backTopLeft, windowColor);
            insertVec3WithColor(m_vertexData, backBottomRight, windowColor);
            insertVec3WithColor(m_vertexData, backBottomLeft, windowColor);
        }
    }
}
