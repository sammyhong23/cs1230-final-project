#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "shape.h"

class Cylinder: public Shape
{
public:
    void updateParams(int param1, int param2) override;
    std::vector<GLfloat> generateShape() override;

private:
    void setVertexData();

    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight,
                  glm::vec2 topLeftUV,
                  glm::vec2 topRightUV,
                  glm::vec2 bottomLeftUV,
                  glm::vec2 bottomRightUV,
                  bool isCap);
    void makeWedge(float curTheta, float nextTheta);
    void makeCylinder();

    std::vector<GLfloat> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};
