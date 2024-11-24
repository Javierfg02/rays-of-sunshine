#include "Sphere.h"
#include <iostream>

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Calculate normals for each vertex (this part is correct)
    glm::vec3 normalTopLeft = glm::normalize(topLeft);
    glm::vec3 normalTopRight = glm::normalize(topRight);
    glm::vec3 normalBottomRight = glm::normalize(bottomRight);
    glm::vec3 normalBottomLeft = glm::normalize(bottomLeft);

    // First triangle: topLeft -> topRight -> bottomLeft
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalTopRight);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalBottomLeft);

    // Second triangle: bottomLeft -> topRight -> bottomRight
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalBottomLeft);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalTopRight);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    float radius = 0.5;
    float phiStep = glm::radians(180.f) / static_cast<float>(m_param1);

    for(int i = 0; i < m_param1; i++) {
        float currentPhi = glm::radians(0.5f) + i * phiStep;
        float nextPhi = glm::radians(0.5f) + (i + 1) * phiStep;


        glm::vec3 topLeft = glm::vec3(
            radius * glm::sin(currentPhi) * glm::cos(currentTheta),
            radius * glm::cos(currentPhi),
            radius * glm::sin(currentPhi) * glm::sin(currentTheta)
            );

        glm::vec3 topRight = glm::vec3(
            radius * glm::sin(currentPhi) * glm::cos(nextTheta),
            radius * glm::cos(currentPhi),
            radius * glm::sin(currentPhi) * glm::sin(nextTheta)
            );

        glm::vec3 bottomLeft = glm::vec3(
            radius * glm::sin(nextPhi) * glm::cos(currentTheta),
            radius * glm::cos(nextPhi),
            radius * glm::sin(nextPhi) * glm::sin(currentTheta)
            );

        glm::vec3 bottomRight = glm::vec3(
            radius * glm::sin(nextPhi) * glm::cos(nextTheta),
            radius * glm::cos(nextPhi),
            radius * glm::sin(nextPhi) * glm::sin(nextTheta)
            );

        makeTile(topLeft, topRight, bottomLeft, bottomRight);
    }
}

void Sphere::makeSphere() {
    // Task 7: create a full sphere using the makeWedge() function you
    //         implemented in Task 6
    // Note: think about how param 2 comes into play here!
    float thetaStep = glm::radians(360.f) / static_cast<float>(m_param2);

    // Create wedges around the sphere
    for(int i = 0; i < m_param2; i++) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        makeWedge(currentTheta, nextTheta);
    }
}

void Sphere::setVertexData() {
    // Uncomment these lines to make a wedge for Task 6, then comment them out for Task 7:

    // float thetaStep = glm::radians(360.f) / static_cast<float>(m_param2);
    // float currentTheta = 0.0f;
    // float nextTheta = thetaStep;
    // makeWedge(currentTheta, nextTheta);

    // Uncomment these lines to make sphere for Task 7:
    if (m_param1 < 2) m_param1 = 2;
    if (m_param2 < 3) m_param2 = 3;
    makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
