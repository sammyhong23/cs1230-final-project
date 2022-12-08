#include "Cone.h"
#include "glm/gtc/epsilon.hpp"

std::vector<GLfloat> Cone::generateShape() {
    m_vertexData.clear();
    setVertexData();
    return m_vertexData;
}

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = std::max(param2, 3);
}

void Cone::setVertexData() {
    makeCone();
}

void Cone::makeCone() {
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; ++i) {
        makeWedge(i * thetaStep, (i + 1)* thetaStep);
    }
}

void Cone::makeWedge(float curTheta, float nextTheta) {
    // make conical part
    float y = -.5f;
    float yStep = 1.f / m_param1;
    for (int i = 0; i < m_param1; ++i) {
        float scale = 1.f - ((y + .5) / 1.f);
        glm::vec3 bottomLeft = glm::vec3(scale * m_radius * sin(curTheta), y, scale * m_radius * cos(curTheta));
        glm::vec3 bottomRight = glm::vec3(scale * m_radius * sin(nextTheta), y, scale * m_radius * cos(nextTheta));
        y += yStep;
        scale = 1.f - ((y + .5) / 1.f);
        glm::vec3 topLeft = glm::vec3(scale * m_radius * sin(curTheta), y, scale * m_radius * cos(curTheta));
        glm::vec3 topRight = glm::vec3(scale * m_radius * sin(nextTheta), y, scale * m_radius * cos(nextTheta));
        makeTile(topLeft, topRight, bottomLeft, bottomRight, false);
    }

    // make cap
    float r = 0.f;
    float rStep = m_radius / m_param1;
    for (int i = 0; i < m_param1; ++i) {
        glm::vec3 bottomLeft = glm::vec3(r * sin(curTheta), -.5f, r * cos(curTheta));
        glm::vec3 bottomRight = glm::vec3(r * sin(nextTheta), -.5f, r * cos(nextTheta));
        r += rStep;
        glm::vec3 topLeft = glm::vec3(r * sin(curTheta), -.5f, r * cos(curTheta));
        glm::vec3 topRight = glm::vec3(r * sin(nextTheta), -.5f, r * cos(nextTheta));

        makeTile(topLeft, topRight, bottomLeft, bottomRight, true);
    }
}

void Cone::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight,
                    bool isCap) {

    glm::vec3 p1t1 = topLeft;
    glm::vec3 p2t1 = bottomLeft;
    glm::vec3 p3t1 = bottomRight;

    glm::vec3 n1t1 = (isCap) ? glm::normalize(glm::cross(p2t1 - p1t1, p3t1 - p1t1)) :
                               glm::normalize(glm::vec3(2 * p1t1[0], (-p1t1[1] + .5f) / 2.f, 2 * p1t1[2]));
    glm::vec3 n2t1 = (isCap) ? glm::normalize(glm::cross(p2t1 - p1t1, p3t1 - p1t1)) :
                               glm::normalize(glm::vec3(2 * p2t1[0], (-p2t1[1] + .5f) / 2.f, 2 * p2t1[2]));
    glm::vec3 n3t1 = (isCap) ? glm::normalize(glm::cross(p2t1 - p1t1, p3t1 - p1t1)) :
                               glm::normalize(glm::vec3(2 * p3t1[0], (-p3t1[1] + .5f) / 2.f, 2 * p3t1[2]));


    glm::vec3 p1t2 = topLeft;
    glm::vec3 p2t2 = bottomRight;
    glm::vec3 p3t2 = topRight;

    glm::vec3 n1t2 = (isCap) ? glm::normalize(glm::cross(p2t2 - p1t2, p3t2 - p1t2)) :
                               glm::normalize(glm::vec3(2 * p1t2[0], (-p1t2[1] + .5f) / 2.f, 2 * p1t2[2]));
    glm::vec3 n2t2 = (isCap) ? glm::normalize(glm::cross(p2t2 - p1t2, p3t2 - p1t2)) :
                               glm::normalize(glm::vec3(2 * p2t2[0], (-p2t2[1] + .5f) / 2.f, 2 * p2t2[2]));
    glm::vec3 n3t2 = (isCap) ? glm::normalize(glm::cross(p2t2 - p1t2, p3t2 - p1t2)) :
                               glm::normalize(glm::vec3(2 * p3t2[0], (-p3t2[1] + .5f) / 2.f, 2 * p3t2[2]));

    // fix tip normals
    glm::vec3 tip {0.f, 0.5, 0.f};
    if (glm::epsilonEqual(glm::length(topLeft - tip), 0.f, 0.0001f)) {
        n1t1 = (n2t2 + n2t1) / 2.f;
        n1t2 = (n2t2 + n2t1) / 2.f;
    }

    Shape::insertVec3(m_vertexData, p1t1);
    Shape::insertVec3(m_vertexData, n1t1);
    Shape::insertVec3(m_vertexData, p2t1);
    Shape::insertVec3(m_vertexData, n2t1);
    Shape::insertVec3(m_vertexData, p3t1);
    Shape::insertVec3(m_vertexData, n3t1);
    Shape::insertVec3(m_vertexData, p1t2);
    Shape::insertVec3(m_vertexData, n1t2);
    Shape::insertVec3(m_vertexData, p2t2);
    Shape::insertVec3(m_vertexData, n2t2);
    Shape::insertVec3(m_vertexData, p3t2);
    Shape::insertVec3(m_vertexData, n3t2);
}
