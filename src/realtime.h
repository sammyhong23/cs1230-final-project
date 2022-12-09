#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include "utils/sceneparser.h"
#include "camera/camera.h"
#include "shape/Cube.h"
#include "shape/Cone.h"
#include "shape/Cylinder.h"
#include "shape/Sphere.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    QElapsedTimer programTimer;

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;

    Camera camera;

    RenderData renderData;

    bool isGLEWInit = false;

    GLuint shader;
    GLuint vbos[4];
    GLuint vaos[4];

    GLuint flow_shader;
    QImage image;
    GLuint image_texture;
    QImage flow_image;
    GLuint flow_texture;
    GLuint fullscreen_vbo;
    GLuint fullscreen_vao;

    Cube* cube;
    Cone* cone;
    Cylinder* cylinder;
    Sphere* sphere;

    std::vector<std::vector<GLfloat>> shapeData;

    glm::mat4 view;
    glm::mat4 proj;

    void initializeVBOSVAOS();
    void updateAllVBOs();
    void updateVBO(int type);
    void updateViewProj();
    void updateShapeParams();

    void passGlobalData();
    void passCameraData();
    void passShapeData(RenderShapeData& shape);
    void passLightData(SceneLightData& light, int lightNum);

    void clearShaderUniforms();

    enum ShapeType primTypeToShapeType(PrimitiveType type);

    void move(float deltat, float units);
    void rotateview(float deltaX, float deltaY, float sensitivity);
    glm::mat3 rotation_mat3(glm::vec3 axis, float angle);

    float t = 0;
};
