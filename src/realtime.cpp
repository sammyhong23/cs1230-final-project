#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"
#include <glm/gtx/string_cast.hpp>


// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
    cube = new Cube();
    cone = new Cone();
    sphere = new Sphere();
    cylinder = new Cylinder();

    shapeData.assign(4, {});
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();
    // Students: anything requiring OpenGL calls when the program exits should be done here
    glDeleteBuffers(4, vbos);
    glDeleteVertexArrays(4, vaos);

    glDeleteProgram(shader);

    delete cube;
    delete cone;
    delete sphere;
    delete cylinder;

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    isGLEWInit = true;
    glClearColor(0.f, 0.f, 0.f, 1.f);
    shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    updateShapeParams();

    memset(vbos, 0, 4 * sizeof(GLuint));
    memset(vaos, 0, 4 * sizeof(GLuint));

    glGenBuffers(4, vbos);
    glGenVertexArrays(4, vaos);

    initializeVBOSVAOS();

    QImage img = QImage(QString("/Users/justinrhee/Desktop/water.jpeg"))
            .convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1, &texturemap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturemap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    img = QImage(QString("/Users/justinrhee/Desktop/watertex.jpeg"))
            .convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1, &heightmap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, heightmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(shader);
    glUniform1f(glGetUniformLocation(shader, "texturemapon"), false);
    glUniform1f(glGetUniformLocation(shader, "heightmapon"), false);
    glUniform1i(glGetUniformLocation(shader, "heightmap"), 1);
    glUniform1i(glGetUniformLocation(shader, "texturemap"), 0);
    glUniform1f(glGetUniformLocation(shader, "worley"), false);
    glUseProgram(0);
}

void Realtime::paintGL() {
    glUseProgram(shader);

    glUniform2f(glGetUniformLocation(shader, "resolution"), size().width(), size().height());
    glUniform1f(glGetUniformLocation(shader, "frequency"), settings.texGenParam1);
    if (settings.flow) {
        glUniform1f(glGetUniformLocation(shader, "time"), 0);
    }
    glUniform1i(glGetUniformLocation(shader, "parallax"), settings.parallax);
    glUniform1f(glGetUniformLocation(shader, "stretch"), settings.texGenParam2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturemap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, heightmap);

    passCameraData();
    passGlobalData();

    for (auto& shape : renderData.shapes) {
        auto type = primTypeToShapeType(shape.primitive.type);
        glBindVertexArray(vaos[type]);
        passShapeData(shape);
        int lightNum = 0;
        for (auto& light : renderData.lights) {
            if (lightNum == 8) break; // limit to 8 lights
            passLightData(light, lightNum);
            ++lightNum;
        }
        glDrawArrays(GL_TRIANGLES, 0, shapeData[type].size() / 11);
        glBindVertexArray(0);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    camera.setAspectRatio(w, h);
    updateViewProj();
}

void Realtime::sceneChanged() {
    SceneParser::parse(settings.sceneFilePath, renderData);

    camera.setCameraData(renderData.cameraData);
    updateShapeParams();
    updateViewProj();

    if (isGLEWInit) {
        this->makeCurrent();
        clearShaderUniforms();
        updateAllVBOs();
    }

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    updateShapeParams();
    updateViewProj();

    if (isGLEWInit) {
        this->makeCurrent();
        updateAllVBOs();
        glUseProgram(shader);
        glUniform1f(glGetUniformLocation(shader, "heightmapon"), settings.parallax);
        glUseProgram(0);
    }

    update(); // asks for a PaintGL() call to occur
}

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        float sensitivity = 0.003;
        rotateview(deltaX, deltaY, sensitivity);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    float units = 5.f;
    move(deltaTime, units);

    update(); // asks for a PaintGL() call to occur
}

void Realtime::initializeVBOSVAOS() {
    for (int i = 0; i < 4; ++i) {
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
        updateVBO(i);

        glBindVertexArray(vaos[i]);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 44, reinterpret_cast<void*>(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 44, reinterpret_cast<void*>(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 44, reinterpret_cast<void*>(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 44, reinterpret_cast<void*>(8 * sizeof(GLfloat)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Realtime::updateAllVBOs() {
    for (int i = 0; i < 4; ++i) {
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
        updateVBO(i);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Realtime::updateVBO(int type) {
    switch(type) {
    case SHAPE_CUBE:
        shapeData[type] = cube->generateShape();
        break;
    case SHAPE_CONE:
        shapeData[type] = cone->generateShape();
        break;
    case SHAPE_SPHERE:
        shapeData[type] = sphere->generateShape();
        break;
    case SHAPE_CYLINDER:
        shapeData[type] = cylinder->generateShape();
        break;
    default:
        std::cout << "not supposed to be here" << std::endl;
        break;
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * shapeData[type].size(), shapeData[type].data(), GL_STATIC_DRAW);
}

enum ShapeType Realtime::primTypeToShapeType(PrimitiveType type) {
    switch (type) {
    case PrimitiveType::PRIMITIVE_CUBE:
        return SHAPE_CUBE;
    case PrimitiveType::PRIMITIVE_CONE:
        return SHAPE_CONE;
    case PrimitiveType::PRIMITIVE_SPHERE:
        return SHAPE_SPHERE;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        return SHAPE_CYLINDER;
    default:
        return SHAPE_CUBE; // shouldn't get here.
    }
}

void Realtime::updateViewProj() {
    view = camera.getViewMatrix();
    proj = camera.getProjectionMatrix(settings.nearPlane, settings.farPlane);
}

void Realtime::updateShapeParams() {
    cube->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    cone->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    cylinder->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    sphere->updateParams(settings.shapeParameter1, settings.shapeParameter2);
}
