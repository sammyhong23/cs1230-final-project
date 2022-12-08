#pragma once

#include "utils/scenedata.h"
#include <glm/glm.hpp>

// A class representing a virtual camera.

class Camera {
public:
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float near, float far);
    float getAspectRatio() const;
    float getHeightAngle() const; // RADIANS
    float getFocalLength() const;
    float getAperture() const;
    glm::vec4 getLook() const;
    glm::vec4 getUp() const;
    glm::vec4 getPos() const;
    void setLook(glm::vec4 look);
    void setUp(glm::vec4 up);
    void setPos(glm::vec4 pos);
    void setCameraData(SceneCameraData sceneCameraData);
    void setAspectRatio(int width, int height);
private:
    SceneCameraData cameraData;
    float aspectRatio;
};
