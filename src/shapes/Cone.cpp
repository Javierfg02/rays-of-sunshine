#include "Cone.h"

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}


void Cone::setVertexData() {
    // TODO for Project 5: Lights, Camera
    if (m_param2 < 3) m_param2 = 3;
    this->makeCone();
}

void Cone::makeCone() {
    float thetaStep = glm::radians(360.0f) / static_cast<float>(m_param2);

    // make the side
    for(int i = 0; i < m_param2; i++) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i+1) * thetaStep;
        this->makeSide(currentTheta, nextTheta);
    }

    // make the cap
    this->makeCap();
}

void Cone::makeSide(float currentTheta, float nextTheta) {
    float heightStep = 1.0f / static_cast<float>(m_param1);
    float radius = 0.5f;

    for(int i = 0; i < m_param1; i++) {
        float currentHeight = i * heightStep - 0.5f;
        float nextHeight = (i+1) * heightStep - 0.5f;

        // Calculate radii
        float currentRadius = radius * (0.5f - currentHeight) / 1.0f;
        float nextRadius = radius * (0.5f - nextHeight) / 1.0f;

        // Calculate vertices
        glm::vec3 bottomLeft(currentRadius * glm::cos(currentTheta),
                             currentHeight,
                             currentRadius * glm::sin(currentTheta));
        glm::vec3 bottomRight(currentRadius * glm::cos(nextTheta),
                              currentHeight,
                              currentRadius * glm::sin(nextTheta));
        glm::vec3 topLeft(nextRadius * glm::cos(currentTheta),
                          nextHeight,
                          nextRadius * glm::sin(currentTheta));
        glm::vec3 topRight(nextRadius * glm::cos(nextTheta),
                           nextHeight,
                           nextRadius * glm::sin(nextTheta));

        // Each vertex gets ONE normal based on its theta (angle)
        // Slope of 0.5 because radius=0.5 and height=1.0
        float heightFromBase = 1.0f - currentHeight;
        float angle = atan2(0.5f, 1.0f);
        glm::vec3 normalLeft = glm::normalize(glm::vec3(
            cos(currentTheta) * cos(angle),
            sin(angle),
            sin(currentTheta) * cos(angle)));

        glm::vec3 normalRight = glm::normalize(glm::vec3(
            glm::cos(nextTheta),
            sin(angle),
            glm::sin(nextTheta)));

        glm::vec3 tipNormal;
        if(i == m_param1 - 1) {
            tipNormal = glm::normalize(normalLeft + normalRight);
        }

        if(i == m_param1 - 1) {
            insertVec3(m_vertexData, topLeft);
            insertVec3(m_vertexData, tipNormal);
            insertVec3(m_vertexData, topRight);
            insertVec3(m_vertexData, tipNormal);
            insertVec3(m_vertexData, bottomLeft);
            insertVec3(m_vertexData, normalLeft);

            insertVec3(m_vertexData, bottomLeft);
            insertVec3(m_vertexData, normalLeft);
            insertVec3(m_vertexData, topRight);
            insertVec3(m_vertexData, tipNormal);
            insertVec3(m_vertexData, bottomRight);
            insertVec3(m_vertexData, normalRight);
        } else {
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
}

void Cone::makeCap() {
    float radius = 0.5f;
    float radiusStep = radius / static_cast<float>(m_param1);
    float thetaStep = glm::radians(360.0f) / static_cast<float>(m_param2);
    glm::vec3 normal = glm::vec3(0, -1.0f, 0);

    // concentric rings
    for (int r = 0; r < m_param1; r++) {
        float currentRadius = r * radiusStep;
        float nextRadius = (r+1) * radiusStep;

        // theta divisions
        for (int i = 0; i < m_param2; i++) {
            float currentTheta = i * thetaStep;
            float nextTheta = (i+1) * thetaStep;

            glm::vec3 topLeft(currentRadius * glm::cos(currentTheta), -0.5f, currentRadius * glm::sin(currentTheta));
            glm::vec3 topRight(currentRadius * glm::cos(nextTheta), -0.5f, currentRadius * glm::sin(nextTheta));
            glm::vec3 bottomLeft(nextRadius * glm::cos(currentTheta), -0.5f, nextRadius * glm::sin(currentTheta));
            glm::vec3 bottomRight(nextRadius * glm::cos(nextTheta), -0.5f, nextRadius * glm::sin(nextTheta));

            insertVec3(m_vertexData, topLeft);
            insertVec3(m_vertexData, normal);
            insertVec3(m_vertexData, bottomLeft);
            insertVec3(m_vertexData, normal);
            insertVec3(m_vertexData, topRight);
            insertVec3(m_vertexData, normal);

            insertVec3(m_vertexData, topRight);
            insertVec3(m_vertexData, normal);
            insertVec3(m_vertexData, bottomLeft);
            insertVec3(m_vertexData, normal);
            insertVec3(m_vertexData, bottomRight);
            insertVec3(m_vertexData, normal);

        }
    }
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
