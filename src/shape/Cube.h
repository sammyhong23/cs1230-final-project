#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "shape.h"

class Cube: public Shape
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
                  glm::vec2 bottomRightUV);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);

    std::vector<GLfloat> m_vertexData;
    int m_param1;
};
