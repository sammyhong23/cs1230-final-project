#include "Cube.h"

std::vector<GLfloat> Cube::generateShape() {
    m_vertexData.clear();
    setVertexData();
    return m_vertexData;
}

void Cube::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    glm::vec3 p1t1 = topLeft;
    glm::vec3 p2t1 = bottomLeft;
    glm::vec3 p3t1 = bottomRight;
    glm::vec3 nt1 = glm::normalize(glm::cross(p2t1 - p1t1, p3t1 - p1t1));
    glm::vec3 p1t2 = topLeft;
    glm::vec3 p2t2 = bottomRight;
    glm::vec3 p3t2 = topRight;
    glm::vec3 nt2 = glm::normalize(glm::cross(p2t2 - p1t2, p3t2 - p1t2));

    Shape::insertVec3(m_vertexData, p1t1);
    Shape::insertVec3(m_vertexData, nt1);
    Shape::insertVec3(m_vertexData, p2t1);
    Shape::insertVec3(m_vertexData, nt1);
    Shape::insertVec3(m_vertexData, p3t1);
    Shape::insertVec3(m_vertexData, nt1);
    Shape::insertVec3(m_vertexData, p1t2);
    Shape::insertVec3(m_vertexData, nt2);
    Shape::insertVec3(m_vertexData, p2t2);
    Shape::insertVec3(m_vertexData, nt2);
    Shape::insertVec3(m_vertexData, p3t2);
    Shape::insertVec3(m_vertexData, nt2);
}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    glm::vec3 rightStep = (topRight - topLeft) / (float) m_param1;
    glm::vec3 upStep = (topLeft - bottomLeft) / (float) m_param1;
    for (int i = 0; i < m_param1; ++i) {
        for (int j = 0; j < m_param1; ++j) {
            makeTile(bottomLeft + upStep * (float) (i + 1) + rightStep * (float) j,
                     bottomLeft + upStep * (float) (i + 1) + rightStep * (float) (j + 1),
                     bottomLeft + upStep * (float) i + rightStep * (float) j,
                     bottomLeft + upStep * (float) i + rightStep * (float) (j + 1));
        }
    }
}


void Cube::setVertexData() {
     makeFace(glm::vec3(-0.5f,  0.5f,  0.5f),
              glm::vec3( 0.5f,  0.5f,  0.5f),
              glm::vec3(-0.5f, -0.5f,  0.5f),
              glm::vec3( 0.5f, -0.5f,  0.5f));
     makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f,  0.5f, -0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f));
     makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f,  0.5f,  0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f,  0.5f));
     makeFace(glm::vec3( 0.5f,  0.5f,  0.5f),
              glm::vec3( 0.5f,  0.5f, -0.5f),
              glm::vec3( 0.5f, -0.5f,  0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f));
     makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
              glm::vec3( 0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f,  0.5f,  0.5f),
              glm::vec3( 0.5f,  0.5f,  0.5f));
     makeFace(glm::vec3( 0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f),
              glm::vec3( 0.5f, -0.5f,  0.5f),
              glm::vec3(-0.5f, -0.5f,  0.5f));
}
