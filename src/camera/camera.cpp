#include <stdexcept>
#include <glm/glm.hpp>
#include "camera.h"
#include "glm/gtx/transform.hpp"
#include <glm/gtx/string_cast.hpp>
#include <iostream>

/**
 * @brief Camera::getViewMatrix
 * @return the view matrix of the camera
 */
glm::mat4 Camera::getViewMatrix() const {    
    glm::vec3 pos(cameraData.pos);
    glm::vec3 look(cameraData.look);
    glm::vec3 up(cameraData.up);

    glm::vec3 w = -glm::normalize(look);
    glm::vec3 v = glm::normalize(up-glm::dot(up,w)*w);
    glm::vec3 u = glm::cross(v,w);

    glm::mat4 rotate = glm::mat4(glm::transpose(glm::mat3(u,v,w)));
    glm::mat4 translate = glm::translate(-pos);

    return rotate * translate;
}

glm::mat4 Camera::getProjectionMatrix(float near, float far) {
    near = 0.1f;
    far = 50.f;
    float Lh = far * tan(cameraData.heightAngle / 2);
    float Lw = Lh * aspectRatio;
    glm::mat4 scale = glm::mat4(1.f / Lw, 0.f, 0.f, 0.f,
                                0.f, 1.f / Lh, 0.f, 0.f,
                                0.f, 0.f, 1.f / far, 0.f,
                                0.f, 0.f, 0.f, 1.f);
    float c = -near / far;
    glm::mat4 unhinge = glm::mat4(1.f, 0.f, 0.f, 0.f,
                                  0.f, 1.f, 0.f, 0.f,
                                  0.f, 0.f, 1.f / (1.f + c), -1.f,
                                  0.f, 0.f, -c / (1.f + c), 0.f);
    glm::mat4 openGLSpace = glm::mat4(1.f, 0.f, 0.f, 0.f,
                                 0.f, 1.f, 0.f, 0.f,
                                 0.f, 0.f, -2.f, 0.f,
                                 0.f, 0.f, -1.f, 1.f);
    return openGLSpace * unhinge * scale;
}

/**
 * @brief Camera::getAspectRatio
 * @return width / height
 */
float Camera::getAspectRatio() const {
    return aspectRatio;
}

/**
 * @brief Camera::getHeightAngle
 * @return height angle
 */
float Camera::getHeightAngle() const {
    return cameraData.heightAngle;
}

/**
 * @brief Camera::getFocalLength
 * @return focal length
 */
float Camera::getFocalLength() const {
    return cameraData.focalLength;
}

/**
 * @brief Camera::getAperture
 * @return aperture
 */
float Camera::getAperture() const {
    return cameraData.aperture;
}

/**
 * @brief Camera::setCameraData sets the camera data
 * @param sceneCameraData
 */
void Camera::setCameraData(SceneCameraData sceneCameraData) {
    cameraData = sceneCameraData;
}

/**
 * @brief Camera::setAspectRatio sets the camera aspect ratio
 * @param width
 * @param height
 */
void Camera::setAspectRatio(int width, int height) {
    aspectRatio = (float) width / (float) height;
}

glm::vec4 Camera::getLook() const {
    return cameraData.look;
}

glm::vec4 Camera::getUp() const {
    return cameraData.up;
}

glm::vec4 Camera::getPos() const {
    return cameraData.pos;
}

void Camera::setLook(glm::vec4 look) {
    cameraData.look = look;
}

void Camera::setUp(glm::vec4 up) {
    cameraData.up = up;
}

void Camera::setPos(glm::vec4 pos) {
    cameraData.pos = pos;
}
