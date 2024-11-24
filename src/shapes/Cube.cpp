#include "Cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    // all vertices will still share a normal
    glm::vec3 edge1 = bottomLeft - topLeft;
    glm::vec3 edge2 = topRight - topLeft;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

    // now the triangles counter-clockwise
    // first triangle is topLeft -> bottomLeft -> bottomRight
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);

    // Second triangle: bottomRight -> topRight -> topLeft
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {

    glm::vec3 deltaHorizontal = (topRight - topLeft) / static_cast<float>(m_param1);
    glm::vec3 deltaVertical = (bottomLeft - topLeft) / static_cast<float>(m_param1);

    for (int row = 0; row < m_param1; row++) {
        for (int col = 0; col < m_param1; col++) {
            glm::vec3 currentTopLeft = topLeft +
                                       static_cast<float>(col) * deltaHorizontal +
                                       static_cast<float>(row) * deltaVertical;

            glm::vec3 currentTopRight = currentTopLeft + deltaHorizontal;
            glm::vec3 currentBottomLeft = currentTopLeft + deltaVertical;
            glm::vec3 currentBottomRight = currentTopLeft + deltaHorizontal + deltaVertical;

            makeTile(currentTopLeft, currentTopRight, currentBottomLeft, currentBottomRight);
        }
    }
}

void Cube::setVertexData() {
    // Comment out Task 2/3 test code
    // makeTile(glm::vec3(-0.5f,  0.5f, 0.5f),
    //          glm::vec3( 0.5f,  0.5f, 0.5f),
    //          glm::vec3(-0.5f, -0.5f, 0.5f),
    //          glm::vec3( 0.5f, -0.5f, 0.5f));

    // Front face (+z)
    makeFace(glm::vec3(-0.5f,  0.5f,  0.5f),   // top-left
             glm::vec3( 0.5f,  0.5f,  0.5f),    // top-right
             glm::vec3(-0.5f, -0.5f,  0.5f),    // bottom-left
             glm::vec3( 0.5f, -0.5f,  0.5f));   // bottom-right

    // Back face (-z)
    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f));

    // Top face (+y)
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3( 0.5f,  0.5f,  0.5f));

    // Bottom face (-y)
    makeFace(glm::vec3(-0.5f, -0.5f,  0.5f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f));

    // Right face (+x)
    makeFace(glm::vec3( 0.5f,  0.5f,  0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f));

    // Left face (-x)
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f,  0.5f));
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
