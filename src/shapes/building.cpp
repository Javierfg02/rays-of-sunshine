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

    // TODO: add a randomized (or pseudo randomized) texture to each building

    // generate the building
    setVertexData();
}

std::vector<float> Building::generateShape() {
    return m_vertexData;
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
    glm::vec3 frontNormal(0, 0, 1);
    makeFace(frontTopLeft, frontTopRight, frontBottomLeft, frontBottomRight, frontNormal);

    // back face (facing away from camera, z negative)
    glm::vec3 backTopLeft(-width/2, totalHeight, -depth/2);
    glm::vec3 backTopRight(width/2, totalHeight, -depth/2);
    glm::vec3 backBottomLeft(-width/2, 0, -depth/2);
    glm::vec3 backBottomRight(width/2, 0, -depth/2);
    glm::vec3 backNormal(0,0,-1);
    // Note: back face needs reverse winding order
    makeFace(backTopRight, backTopLeft, backBottomRight, backBottomLeft, backNormal);

    // right face (x positive)
    glm::vec3 rightTopLeft(width/2, totalHeight, -depth/2);
    glm::vec3 rightTopRight(width/2, totalHeight, depth/2);
    glm::vec3 rightBottomLeft(width/2, 0, -depth/2);
    glm::vec3 rightBottomRight(width/2, 0, depth/2);
    glm::vec3 rightNormal(1, 0, 0);
    makeFace(rightTopRight, rightTopLeft, rightBottomRight, rightBottomLeft, rightNormal);

    // left face (x negative)
    glm::vec3 leftTopLeft(-width/2, totalHeight, -depth/2);
    glm::vec3 leftTopRight(-width/2, totalHeight, depth/2);
    glm::vec3 leftBottomLeft(-width/2, 0, -depth/2);
    glm::vec3 leftBottomRight(-width/2, 0, depth/2);
    glm::vec3 leftNormal(-1, 0, 0);
    makeFace(leftTopLeft, leftTopRight, leftBottomLeft, leftBottomRight, leftNormal);

    // top face
    glm::vec3 topNormal(0, 1, 0);
    makeFace(frontTopRight, frontTopLeft, backTopRight, backTopLeft, topNormal);

    // bottom face
    glm::vec3 bottomNormal(0, -1, 0);
    makeFace(frontBottomLeft, frontBottomRight, backBottomLeft, backBottomRight, bottomNormal);

    // Add windows after basic structure
    if (m_windowsPerFloor > 0) {
        addWindows();
    }
}

void Building::makeFace(glm::vec3 topLeft, glm::vec3 topRight,
                        glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 normal) {
    glm::vec3 wallColor = settings.wallColor; // Light gray for walls

    glm::vec3 edge1 = bottomRight - bottomLeft;  // First edge from first to second vertex
    glm::vec3 edge2 = topRight - bottomLeft;     // Second edge from first to third vertex
    // glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

    // Draw face using consistent counter-clockwise winding order
    // First triangle
    insertVec3(bottomLeft, normal, wallColor);
    insertVec3(bottomRight, normal, wallColor);
    insertVec3(topRight, normal, wallColor);

    // Second triangle
    insertVec3(bottomLeft, normal, wallColor);
    insertVec3(topRight, normal, wallColor);
    insertVec3(topLeft, normal, wallColor);
}


void Building::addWindows() {
    float windowWidth = settings.windowWidth;
    float windowHeight = settings.windowHeight;
    glm::vec3 regularWindowColor = settings.windowColor;
    glm::vec3 litWindowColor = glm::vec3(2.0f, 1.9f, 1.4f);

    // Space windows evenly across the front face
    float spacing = m_buildingWidth / (m_windowsPerFloor + 1);

    // Use building position to seed window lighting pattern
    for (int floor = 0; floor < m_numFloors; floor++) {
        float floorBase = floor * m_floorHeight;

        for (int window = 0; window < m_windowsPerFloor; window++) {
            float windowX = -m_buildingWidth/2 + spacing * (window + 1);
            float windowY = floorBase + 0.25f;

            // Determine if window should be lit
            bool isLit = (rand() % 100) < 15;  // 30% chance of being lit
            glm::vec3 windowColor = isLit ? litWindowColor : regularWindowColor;

            // Front face windows
            glm::vec3 frontTopLeft(windowX - windowWidth/2, windowY + windowHeight, m_buildingDepth/2 + 0.01f);
            glm::vec3 frontTopRight(windowX + windowWidth/2, windowY + windowHeight, m_buildingDepth/2 + 0.01f);
            glm::vec3 frontBottomLeft(windowX - windowWidth/2, windowY, m_buildingDepth/2 + 0.01f);
            glm::vec3 frontBottomRight(windowX + windowWidth/2, windowY, m_buildingDepth/2 + 0.01f);
            glm::vec3 frontNormal(0, 0, 1);

            // Front windows
            insertVec3(frontTopLeft, frontNormal, windowColor);
            insertVec3(frontBottomLeft, frontNormal, windowColor);
            insertVec3(frontTopRight, frontNormal, windowColor);
            insertVec3(frontTopRight, frontNormal, windowColor);
            insertVec3(frontBottomLeft, frontNormal, windowColor);
            insertVec3(frontBottomRight, frontNormal, windowColor);

            // Back face windows (matching front windows)
            glm::vec3 backTopLeft(windowX - windowWidth/2, windowY + windowHeight, -m_buildingDepth/2 - 0.01f);
            glm::vec3 backTopRight(windowX + windowWidth/2, windowY + windowHeight, -m_buildingDepth/2 - 0.01f);
            glm::vec3 backBottomLeft(windowX - windowWidth/2, windowY, -m_buildingDepth/2 - 0.01f);
            glm::vec3 backBottomRight(windowX + windowWidth/2, windowY, -m_buildingDepth/2 - 0.01f);
            glm::vec3 backNormal(0, 0, -1);

            insertVec3(backTopRight, backNormal, windowColor);
            insertVec3(backBottomRight, backNormal, windowColor);
            insertVec3(backTopLeft, backNormal, windowColor);
            insertVec3(backTopLeft, backNormal, windowColor);
            insertVec3(backBottomRight, backNormal, windowColor);
            insertVec3(backBottomLeft, backNormal, windowColor);

            // left face windows
            float zSpacing = m_buildingDepth / (m_windowsPerFloor + 1);

            // Left face windows
            glm::vec3 leftTopLeft(-m_buildingWidth/2 - 0.01f, windowY + windowHeight, -m_buildingDepth/2 + zSpacing * (window + 1));
            glm::vec3 leftTopRight(-m_buildingWidth/2 - 0.01f, windowY + windowHeight, -m_buildingDepth/2 + zSpacing * (window + 1) + windowWidth);
            glm::vec3 leftBottomLeft(-m_buildingWidth/2 - 0.01f, windowY, -m_buildingDepth/2 + zSpacing * (window + 1));
            glm::vec3 leftBottomRight(-m_buildingWidth/2 - 0.01f, windowY, -m_buildingDepth/2 + zSpacing * (window + 1) + windowWidth);
            glm::vec3 leftNormal(-1, 0, 0);

            insertVec3(leftTopLeft, leftNormal, windowColor);
            insertVec3(leftBottomLeft, leftNormal, windowColor);
            insertVec3(leftTopRight, leftNormal, windowColor);
            insertVec3(leftTopRight, leftNormal, windowColor);
            insertVec3(leftBottomLeft, leftNormal, windowColor);
            insertVec3(leftBottomRight, leftNormal, windowColor);

            // right face windows
            glm::vec3 rightTopLeft(m_buildingWidth/2 + 0.01f, windowY + windowHeight, -m_buildingDepth/2 + zSpacing * (window + 1));
            glm::vec3 rightTopRight(m_buildingWidth/2 + 0.01f, windowY + windowHeight, -m_buildingDepth/2 + zSpacing * (window + 1) + windowWidth);
            glm::vec3 rightBottomLeft(m_buildingWidth/2 + 0.01f, windowY, -m_buildingDepth/2 + zSpacing * (window + 1));
            glm::vec3 rightBottomRight(m_buildingWidth/2 + 0.01f, windowY, -m_buildingDepth/2 + zSpacing * (window + 1) + windowWidth);
            glm::vec3 rightNormal(1, 0, 0);

            insertVec3(rightTopRight, rightNormal, windowColor);
            insertVec3(rightBottomRight, rightNormal, windowColor);
            insertVec3(rightTopLeft, rightNormal, windowColor);
            insertVec3(rightTopLeft, rightNormal, windowColor);
            insertVec3(rightBottomRight, rightNormal, windowColor);
            insertVec3(rightBottomLeft, rightNormal, windowColor);
        }
    }
}
