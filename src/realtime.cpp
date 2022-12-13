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
    texgenshader = ShaderLoader::createShaderProgram(":/resources/shaders/texgen.vert", ":/resources/shaders/texgen.frag");
    updateShapeParams();

    memset(vbos, 0, 4 * sizeof(GLuint));
    memset(vaos, 0, 4 * sizeof(GLuint));

    glGenBuffers(4, vbos);
    glGenVertexArrays(4, vaos);

    initializeVBOSVAOS();

    QImage img = QImage(QString("/Users/justinrhee/Desktop/images/tiled_water2.png"))
            .convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1, &texturemap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturemap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    img = QImage(QString("/Users/justinrhee/Desktop/watercolor.jpeg"))
            .convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1, &heightmap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, heightmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    img = QImage(QString("/Users/justinrhee/Desktop/fm2.png"))
            .convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1, &flowmap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, flowmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    flowtimer.restart();

    glUseProgram(shader);
    glUniform1f(glGetUniformLocation(shader, "texturemapon"), false);
    glUniform1f(glGetUniformLocation(shader, "heightmapon"), false);
    glUniform1i(glGetUniformLocation(shader, "heightmap"), 1);
    glUniform1i(glGetUniformLocation(shader, "texturemap"), 0);
    glUniform1i(glGetUniformLocation(shader, "flowMap"), 2);
    glUniform1f(glGetUniformLocation(shader, "worley"), false);
    glUniform1f(glGetUniformLocation(shader, "flowSpeed"), 1.f / 10000.f);
    glUniform1f(glGetUniformLocation(shader, "time"), 0);
    glUseProgram(0);

    glUseProgram(texgenshader);
    glUniform2f(glGetUniformLocation(texgenshader, "resolution"), size().width(), size().height());
    glUniform1f(glGetUniformLocation(texgenshader, "worley"), settings.worley);
    glUniform1f(glGetUniformLocation(texgenshader, "frequency"), settings.texGenParam1);
    glUniform1f(glGetUniformLocation(texgenshader, "stretch"), settings.texGenParam2);
    glUseProgram(0);

    makeFBO(&fbo, &fbotex, &fborenderbuff);
    setupFullScreenQuad();
}

void Realtime::paintGL() {
    glBindFramebuffer(GL_FRAMEBUFFER, DEFAULT_FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);

    if (settings.flow) {
        glUniform1f(glGetUniformLocation(shader, "time"), flowtimer.elapsed());
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturemap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fbotex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, flowmap);

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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    glUseProgram(shader);
    glUniform2f(glGetUniformLocation(shader, "resolution"), size().width(), size().height());
    glUseProgram(0);

    glUseProgram(texgenshader);
    glUniform2f(glGetUniformLocation(texgenshader, "resolution"), size().width(), size().height());
    glUseProgram(0);

    glDeleteTextures(1, &fbotex);
    glDeleteRenderbuffers(1, &fborenderbuff);
    glDeleteFramebuffers(1, &fbo);

    makeFBO(&fbo, &fbotex, &fborenderbuff);

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
        glUniform1f(glGetUniformLocation(shader, "time"), 0);
        glUniform1f(glGetUniformLocation(shader, "heightmapon"), settings.parallax);
        glUniform1f(glGetUniformLocation(shader, "worley"), settings.worley);
        glUniform1f(glGetUniformLocation(shader, "frequency"), settings.texGenParam1);
        glUniform1f(glGetUniformLocation(shader, "stretch"), settings.texGenParam2);
        glUniform1f(glGetUniformLocation(shader, "flowSpeed"), (float) settings.flowspeed / 10000.f);
        glUseProgram(0);

        glUseProgram(texgenshader);
        glUniform1f(glGetUniformLocation(texgenshader, "worley"), settings.worley);
        glUniform1f(glGetUniformLocation(texgenshader, "frequency"), settings.texGenParam1);
        glUniform1f(glGetUniformLocation(texgenshader, "stretch"), settings.texGenParam2);
        glUseProgram(0);

        paintFBO(fbo, texgenshader);
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

    if (settings.bezier) {
        std::vector<float> xX1{12.0f, 15.0f, 23.f, 26.0f};
        std::vector<float> yY1{6.0f, 6.0f, 6.0f, 6.0f};
        std::vector<float> zZ1{6.0f, -2.0f, -2.0f, 6.0f};

        std::vector<float> xX2{26.f, 29.0f, 37.0f, 40.0f};
        std::vector<float> yY2{6.0f, 6.0f, 6.0f, 6.0f};
        std::vector<float> zZ2{6.f, 8.0f, 9.0f, 6.f};

        std::tuple<std::vector<float>, std::vector<float>, std::vector<float>> bCurve3D1 = computeBesierCurve3D(xX1, yY1, zZ1);
        std::tuple<std::vector<float>, std::vector<float>, std::vector<float>> bCurve3D2 = computeBesierCurve3D(xX2, yY2, zZ2);

        std::vector<float> vecX1 = get<0>(bCurve3D1);
        std::vector<float> vecY1 = get<1>(bCurve3D1);
        std::vector<float> vecZ1 = get<2>(bCurve3D1);
        std::vector<float> vecX2 = get<0>(bCurve3D2);
        std::vector<float> vecY2 = get<1>(bCurve3D2);
        std::vector<float> vecZ2 = get<2>(bCurve3D2);

        vecX1.insert( vecX1.end(), vecX2.begin(), vecX2.end() );
        vecY1.insert( vecY1.end(), vecY2.begin(), vecY2.end() );
        vecZ1.insert( vecZ1.end(), vecZ2.begin(), vecZ2.end() );

        std::tuple<std::vector<float>,
                std::vector<float>,
                std::vector<float>> bCurveCombined = std::make_tuple(vecX1, vecY1, vecZ1);

        move2(curveLength, bCurveCombined);
    } else {
        float units = 5.f;
        move(deltaTime, units);
    }

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

void Realtime::makeFBO(GLuint* fbo, GLuint* fbo_texture, GLuint* fbo_renderbuffer) {
    glGenTextures(1, fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size().width(), size().height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, *fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size().width(), size().height());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *fbo_renderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Realtime::setupFullScreenQuad() {
    std::vector<GLfloat> fullscreen_quad_data =
    {
        -1.f,  1.f, 0.0f, 0.f, 1.f,
        -1.f, -1.f, 0.0f, 0.f, 0.f,
        1.f, -1.f, 0.0f, 1.f, 0.f,
        1.f,  1.f, 0.0f, 1.f, 1.f,
        -1.f,  1.f, 0.0f, 0.f, 1.f,
        1.f, -1.f, 0.0f, 1.f, 0.f
    };

    glGenBuffers(1, &fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &fullscreen_vao);
    glBindVertexArray(fullscreen_vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(0 * sizeof(GLfloat)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::paintFBO(const GLuint& fbo, const GLuint& shaderid) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //glViewport(0, 0, size().width(), size().height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderid);
    glBindVertexArray(fullscreen_vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

std::tuple<std::vector<float>, std::vector<float>, std::vector<float>> Realtime::computeBesierCurve3D(std::vector<float> xX,
                                                                                                      std::vector<float> yY,
                                                                                                      std::vector<float> zZ) {
    std::vector<float> bCurveX;
    std::vector<float> bCurveY;
    std::vector<float> bCurveZ;
    float bCurveXt;
    float bCurveYt;
    float bCurveZt;

    for (float t = 0.005f; t <= 1.f; t += 0.01f)
    {

        bCurveXt = std::pow((1 - t), 3) * xX[0] + 3 * std::pow((1 - t), 2) * t * xX[1] + 3 * std::pow((1 - t), 1) * std::pow(t, 2) * xX[2] + std::pow(t, 3) * xX[3];
        bCurveYt = std::pow((1 - t), 3) * yY[0] + 3 * std::pow((1 - t), 2) * t * yY[1] + 3 * std::pow((1 - t), 1) * std::pow(t, 2) * yY[2] + std::pow(t, 3) * yY[3];
        bCurveZt = std::pow((1 - t), 3) * zZ[0] + 3 * std::pow((1 - t), 2) * t * zZ[1] + 3 * std::pow((1 - t), 1) * std::pow(t, 2) * yY[2] + std::pow(t, 3) * zZ[3];

        bCurveX.push_back(bCurveXt);
        bCurveY.push_back(bCurveYt);
        bCurveZ.push_back(bCurveZt);
    }

    return std::make_tuple(bCurveX, bCurveY, bCurveZ);
}
