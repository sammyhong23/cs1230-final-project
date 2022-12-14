#ifndef SHAPE_H
#define SHAPE_H

#include "GL/glew.h"
#include <glm/glm.hpp>
#include <vector>

enum ShapeType {
    SHAPE_CUBE,
    SHAPE_CONE,
    SHAPE_SPHERE,
    SHAPE_CYLINDER
};

class Shape
{
public:
    virtual void updateParams(int param1, int param2) = 0;
    virtual std::vector<GLfloat> generateShape() = 0;

    void insertVec3(std::vector<GLfloat> &data, glm::vec3 v) {
        data.push_back(v.x);
        data.push_back(v.y);
        data.push_back(v.z);
    }

    void insertVec2(std::vector<GLfloat> &data, glm::vec2 v) {
        data.push_back(v[0]);
        data.push_back(v[1]);
    }
};

#endif // SHAPE_H
