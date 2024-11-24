#include "Cylinder.h"
#include <iostream>

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

void Cylinder::setVertexData() {
    if(m_param2 < 3) m_param2 = 3;
    makeCylinder();
}

void Cylinder::makeCylinder() {
    float thetaStep = glm::radians(360.0f) / static_cast<float>(m_param2);

    // make the sides
    for(int i = 0; i < m_param2; i++) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i+1) * thetaStep;
        this->makeSide(currentTheta, nextTheta);
    }

    // make the caps
    makeCap(true);
    makeCap(false);
}

void Cylinder::makeSide(float currentTheta, float nextTheta) {
    float heightStep = 1.0f / static_cast<float>(m_param1);
    float radius = 0.5f;

    for(int i = 0; i < m_param1; i++) {
        float currentHeight = i * heightStep - 0.5f;
        float nextHeight = (i+1) * heightStep - 0.5f;

        // calculate the corners of the cross section
        glm::vec3 topLeft = glm::vec3(radius * glm::cos(currentTheta),
                                      nextHeight,
                                      radius * glm::sin(currentTheta));

        glm::vec3 bottomLeft = glm::vec3(radius * glm::cos(currentTheta),
                                         currentHeight,
                                         radius * glm::sin(currentTheta));

        glm::vec3 bottomRight = glm::vec3(radius * glm::cos(nextTheta),
                                          currentHeight,
                                          radius * glm::sin(nextTheta));
        glm::vec3 topRight = glm::vec3(radius * glm::cos(nextTheta),
                                       nextHeight,
                                       radius * glm::sin(nextTheta));

        // calculate the normals (which point out from the center for the side)
        glm::vec3 normalLeft = glm::normalize(glm::vec3(glm::cos(currentTheta),
                                                        0,
                                                        glm::sin(currentTheta)));
        glm::vec3 normalRight = glm::normalize(glm::vec3(glm::cos(nextTheta),
                                                         0,
                                                         glm::sin(nextTheta)));

        // now insert the triangles into the data
        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normalLeft);
        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normalRight);
        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normalLeft);

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normalLeft);
        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normalRight);
        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normalRight);
    }
}

void Cylinder::makeCap(bool isTop) {
    float y = isTop ? 0.5f : -0.5f;
    glm::vec3 normal = isTop ? glm::vec3(0, 1.0f, 0) : glm::vec3(0, -1.0f, 0);
    float radius = 0.5f;
    float thetaStep = glm::radians(360.0f) / static_cast<float>(m_param2);
    float radiusStep = radius / static_cast<float>(m_param1);

    // I am guessing that for smoother shading we have concentric rings
    for(int r = 0; r < m_param1; r++) {
        float currentRadius = r * radiusStep;
        float nextRadius = (r + 1) * radiusStep;

        // make wedges around each ring
        for(int i = 0; i < m_param2; i++) {
            float currentTheta = i * thetaStep;
            float nextTheta = (i + 1) * thetaStep;

            glm::vec3 innerCurrent(currentRadius * glm::cos(currentTheta), y, currentRadius * glm::sin(currentTheta));
            glm::vec3 innerNext(currentRadius * glm::cos(nextTheta), y, currentRadius * glm::sin(nextTheta));
            glm::vec3 outerCurrent(nextRadius * glm::cos(currentTheta), y, nextRadius * glm::sin(currentTheta));
            glm::vec3 outerNext(nextRadius * glm::cos(nextTheta), y, nextRadius * glm::sin(nextTheta));

            if(isTop) {
                insertVec3(m_vertexData, innerCurrent);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, innerNext);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, outerNext);
                insertVec3(m_vertexData, normal);

                insertVec3(m_vertexData, innerCurrent);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, outerNext);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, outerCurrent);
                insertVec3(m_vertexData, normal);
            } else {
                insertVec3(m_vertexData, innerCurrent);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, outerCurrent);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, innerNext);
                insertVec3(m_vertexData, normal);

                insertVec3(m_vertexData, innerNext);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, outerCurrent);
                insertVec3(m_vertexData, normal);
                insertVec3(m_vertexData, outerNext);
                insertVec3(m_vertexData, normal);
            }
        }
    }
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}


