#include "Sphere.h"

std::vector<GLfloat> Sphere::generateShape() {
    setVertexData();
    return m_vertexData;
}

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = std::max(param1, 2);
    m_param2 = std::max(param2, 3);
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight,
                      glm::vec2 topLeftUV,
                      glm::vec2 topRightUV,
                      glm::vec2 bottomLeftUV,
                      glm::vec2 bottomRightUV) {
    glm::vec3 p1t1 = topLeft;
    glm::vec3 p2t1 = bottomLeft;
    glm::vec3 p3t1 = bottomRight;
    glm::vec3 n1t1 = glm::normalize(p1t1);
    glm::vec3 n2t1 = glm::normalize(p2t1);
    glm::vec3 n3t1 = glm::normalize(p3t1);
    glm::vec3 p1t2 = topLeft;
    glm::vec3 p2t2 = bottomRight;
    glm::vec3 p3t2 = topRight;
    glm::vec3 n1t2 = glm::normalize(p1t2);
    glm::vec3 n2t2 = glm::normalize(p2t2);
    glm::vec3 n3t2 = glm::normalize(p3t2);

    Shape::insertVec3(m_vertexData, p1t1);
    Shape::insertVec3(m_vertexData, n1t1);
    Shape::insertVec2(m_vertexData, topLeftUV);

    Shape::insertVec3(m_vertexData, p2t1);
    Shape::insertVec3(m_vertexData, n2t1);
    Shape::insertVec2(m_vertexData, bottomLeftUV);

    Shape::insertVec3(m_vertexData, p3t1);
    Shape::insertVec3(m_vertexData, n3t1);
    Shape::insertVec2(m_vertexData, bottomRightUV);

    Shape::insertVec3(m_vertexData, p1t2);
    Shape::insertVec3(m_vertexData, n1t2);
    Shape::insertVec2(m_vertexData, topLeftUV);

    Shape::insertVec3(m_vertexData, p2t2);
    Shape::insertVec3(m_vertexData, n2t2);
    Shape::insertVec2(m_vertexData, bottomRightUV);

    Shape::insertVec3(m_vertexData, p3t2);
    Shape::insertVec3(m_vertexData, n3t2);
    Shape::insertVec2(m_vertexData, topRightUV);
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    float phi = 0;
    float phiStep = M_PI / (float) m_param1;

    float uStart = currentTheta / (2 * M_PI);
    float uStep = fabs(nextTheta - currentTheta) / (2 * M_PI);
    float vStep = 1.f / (float) m_param1;

    for (int i = 0; i < m_param1; ++i) {
        glm::vec3 topLeft = m_radius * glm::vec3(sin(phi) * sin(currentTheta), cos(phi), sin(phi) * cos(currentTheta));
        glm::vec3 topRight = m_radius * glm::vec3(sin(phi) * sin(nextTheta), cos(phi), sin(phi) * cos(nextTheta));
        phi += phiStep;
        glm::vec3 bottomLeft = m_radius * glm::vec3(sin(phi) * sin(currentTheta), cos(phi), sin(phi) * cos(currentTheta));
        glm::vec3 bottomRight = m_radius * glm::vec3(sin(phi) * sin(nextTheta), cos(phi), sin(phi) * cos(nextTheta));

        glm::vec2 topLeftUV = glm::vec2(uStart, vStep * (float) (i + 1));
        glm::vec2 topRightUV = glm::vec2(uStart + uStep, vStep * (float) (i + 1));
        glm::vec2 bottomLeftUV = glm::vec2(uStart, vStep * (float) i);
        glm::vec2 bottomRightUV = glm::vec2(uStart + uStep, vStep * (float) i);

        makeTile(topLeft, topRight, bottomLeft, bottomRight, topLeftUV, topRightUV, bottomLeftUV, bottomRightUV);
    }
}

void Sphere::makeSphere() {
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; ++i) {
        makeWedge(i * thetaStep, (i + 1)* thetaStep);
    }
}

void Sphere::setVertexData() {
    makeSphere();
}
