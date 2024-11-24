#include "Triangle.h"

void Triangle::updateParams() {
    m_vertexData = std::vector<float>();
    setVertexData();
}

void Triangle::setVertexData() {
    // Task 1: update m_vertexData with the vertices and normals
    //         needed to tesselate a triangle
    // Note: you may find the insertVec3 function useful in adding your points into m_vertexData
    glm::vec3 v_a(-0.5, 0.5, 0.0);
    glm::vec3 v_b(-0.5, -0.5, 0.0);
    glm::vec3 v_c(0.5, -0.5, 0.0);

    // only one triangle, so only one normal repeated for all vertices
    glm::vec3 normal = glm::normalize(glm::cross(v_b - v_a, v_c - v_a));

    insertVec3(m_vertexData, v_a);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, v_b);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, v_c);
    insertVec3(m_vertexData, normal);
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Triangle::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
