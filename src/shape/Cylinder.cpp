#include "Cylinder.h"

std::vector<GLfloat> Cylinder::generateShape() {
    setVertexData();
    return m_vertexData;
}

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = std::max(param2, 3);
}

void Cylinder::setVertexData() {
    makeCylinder();
}

void Cylinder::makeCylinder() {
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; ++i) {
        makeWedge(i * thetaStep, (i + 1)* thetaStep);
    }
}

void Cylinder::makeWedge(float curTheta, float nextTheta) {
    // make circular part
    float y = -.5f;
    float yStep = 1.f / m_param1;

    float uStart = curTheta / (2 * M_PI);
    float uStep = fabs(nextTheta - curTheta) / (2 * M_PI);
    float vStep = yStep;

    for (int i = 0; i < m_param1; ++i) {
        glm::vec3 bottomLeft = glm::vec3(m_radius * sin(curTheta), y, m_radius * cos(curTheta));
        glm::vec3 bottomRight = glm::vec3(m_radius * sin(nextTheta), y, m_radius * cos(nextTheta));
        y += yStep;
        glm::vec3 topLeft = glm::vec3(m_radius * sin(curTheta), y, m_radius * cos(curTheta));
        glm::vec3 topRight = glm::vec3(m_radius * sin(nextTheta), y, m_radius * cos(nextTheta));

        glm::vec2 topLeftUV = glm::vec2(uStart, vStep * (float) (i + 1));
        glm::vec2 topRightUV = glm::vec2(uStart + uStep, vStep * (float) (i + 1));
        glm::vec2 bottomLeftUV = glm::vec2(uStart, vStep * (float) i);
        glm::vec2 bottomRightUV = glm::vec2(uStart + uStep, vStep * (float) i);
        makeTile(topLeft,
                 topRight,
                 bottomLeft,
                 bottomRight,
                 topLeftUV,
                 topRightUV,
                 bottomLeftUV,
                 bottomRightUV,
                 false);
    }

    // make caps
    float r = 0.f;
    float rStep = m_radius / m_param1;
    for (int i = 0; i < m_param1; ++i) {
        glm::vec3 topLeft1 = glm::vec3(r * sin(curTheta), .5f, r * cos(curTheta));
        glm::vec3 topRight1 = glm::vec3(r * sin(nextTheta), .5f, r * cos(nextTheta));
        glm::vec3 bottomLeft2 = glm::vec3(r * sin(curTheta), -.5f, r * cos(curTheta));
        glm::vec3 bottomRight2 = glm::vec3(r * sin(nextTheta), -.5f, r * cos(nextTheta));
        r += rStep;
        glm::vec3 bottomLeft1 = glm::vec3(r * sin(curTheta), .5f, r * cos(curTheta));
        glm::vec3 bottomRight1 = glm::vec3(r * sin(nextTheta), .5f, r * cos(nextTheta));
        glm::vec3 topLeft2 = glm::vec3(r * sin(curTheta), -.5f, r * cos(curTheta));
        glm::vec3 topRight2 = glm::vec3(r * sin(nextTheta), -.5f, r * cos(nextTheta));

        glm::vec2 topLeftUV1 = glm::vec2(topLeft1[0], -topLeft1[2]) + .5f;
        glm::vec2 topRightUV1 = glm::vec2(topRight1[0], -topRight1[2]) + .5f;
        glm::vec2 bottomLeftUV1 = glm::vec2(bottomLeft1[0], -bottomLeft1[2]) + .5f;
        glm::vec2 bottomRightUV1 = glm::vec2(bottomRight1[0], -bottomRight1[2]) + .5f;

        glm::vec2 topLeftUV2 = glm::vec2(topLeft2[0], -topLeft2[2]) + .5f;
        glm::vec2 topRightUV2 = glm::vec2(topRight2[0], -topRight2[2]) + .5f;
        glm::vec2 bottomLeftUV2 = glm::vec2(bottomLeft2[0], -bottomLeft2[2]) + .5f;
        glm::vec2 bottomRightUV2 = glm::vec2(bottomRight2[0], -bottomRight2[2]) + .5f;

        makeTile(topLeft1,
                 topRight1,
                 bottomLeft1,
                 bottomRight1,
                 topLeftUV1,
                 topRightUV1,
                 bottomLeftUV1,
                 bottomRightUV1,
                 true); // top cap
        makeTile(topLeft2,
                 topRight2,
                 bottomLeft2,
                 bottomRight2,
                 topLeftUV2,
                 topRightUV2,
                 bottomLeftUV2,
                 bottomRightUV2,
                 true); // bottom cap
    }
}

void Cylinder::makeTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight,
                        glm::vec2 topLeftUV,
                        glm::vec2 topRightUV,
                        glm::vec2 bottomLeftUV,
                        glm::vec2 bottomRightUV,
                        bool isCap) {

    glm::vec3 p1t1 = topLeft;
    glm::vec3 p2t1 = bottomLeft;
    glm::vec3 p3t1 = bottomRight;

    glm::vec3 n1t1 = (isCap) ? glm::normalize(glm::cross(p2t1 - p1t1, p3t1 - p1t1)) :
                               glm::normalize(glm::vec3(p1t1[0], 0.f, p1t1[2]));
    glm::vec3 n2t1 = (isCap) ? glm::normalize(glm::cross(p2t1 - p1t1, p3t1 - p1t1)) :
                               glm::normalize(glm::vec3(p2t1[0], 0.f, p2t1[2]));
    glm::vec3 n3t1 = (isCap) ? glm::normalize(glm::cross(p2t1 - p1t1, p3t1 - p1t1)) :
                               glm::normalize(glm::vec3(p3t1[0], 0.f, p3t1[2]));

    glm::vec3 p1t2 = topLeft;
    glm::vec3 p2t2 = bottomRight;
    glm::vec3 p3t2 = topRight;

    glm::vec3 n1t2 = (isCap) ? glm::normalize(glm::cross(p2t2 - p1t2, p3t2 - p1t2)) :
                               glm::normalize(glm::vec3(p1t2[0], 0.f, p1t2[2]));
    glm::vec3 n2t2 = (isCap) ? glm::normalize(glm::cross(p2t2 - p1t2, p3t2 - p1t2)) :
                               glm::normalize(glm::vec3(p2t2[0], 0.f, p2t2[2]));
    glm::vec3 n3t2 = (isCap) ? glm::normalize(glm::cross(p2t2 - p1t2, p3t2 - p1t2)) :
                               glm::normalize(glm::vec3(p3t2[0], 0.f, p3t2[2]));

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
