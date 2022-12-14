#include <realtime.h>

void Realtime::move(float deltat, float units) {
    glm::vec3 moveDir = glm::vec3(0.f);
    glm::vec3 wDir = glm::normalize(glm::vec3(camera.getLook()));
    glm::vec3 sDir = -wDir;
    glm::vec3 aDir = glm::normalize(glm::cross(glm::vec3(camera.getUp()), glm::vec3(camera.getLook())));
    glm::vec3 dDir = -aDir;
    glm::vec3 spaceDir = glm::vec3(0,1,0);
    glm::vec3 ctrlDir = -spaceDir;

    if (m_keyMap[Qt::Key_W]) {
        moveDir += wDir;
    }
    if (m_keyMap[Qt::Key_S]) {
        moveDir += sDir;
    }
    if (m_keyMap[Qt::Key_A]) {
        moveDir += aDir;
    }
    if (m_keyMap[Qt::Key_D]) {
        moveDir += dDir;
    }
    if (m_keyMap[Qt::Key_Space]) {
        moveDir += spaceDir;
    }
    if (m_keyMap[Qt::Key_Control] || m_keyMap[Qt::Key_Meta]) {
        moveDir += ctrlDir;
    }
    if (glm::length(moveDir) != 0) {
        moveDir = glm::normalize(moveDir);
    }

    glm::vec4 newCameraPos = camera.getPos() + glm::vec4(units * moveDir * deltat, 0.f);
    camera.setPos(newCameraPos);
    updateViewProj();
}

void Realtime::move2(int i, std::tuple<std::vector<float>, std::vector<float>, std::vector<float>> bCurve3D) {
    if (m_keyMap[Qt::Key_B] && curveLength < get<0>(bCurve3D).size()) {
        glm::vec4 newCameraPos = glm::vec4(get<0>(bCurve3D)[i], get<1>(bCurve3D)[i], get<2>(bCurve3D)[i], 1.f);
        curveLength++;
        camera.setPos(newCameraPos);
        camera.setLook(-glm::normalize(newCameraPos));
        camera.setUp(glm::vec4(0,1,0,0));
        updateViewProj();
    }
    if (m_keyMap[Qt::Key_F]) {
        curveLength = 0;
        glm::vec4 newCameraPos = glm::vec4(get<0>(bCurve3D)[i], get<1>(bCurve3D)[i], get<2>(bCurve3D)[i], 1.f);
        curveLength++;
        camera.setPos(newCameraPos);
        camera.setLook(-glm::normalize(newCameraPos));
        camera.setUp(glm::vec4(0,1,0,0));
        updateViewProj();
    }
}

void Realtime::rotateview(float deltaX, float deltaY, float sensitivity) {
    glm::vec3 xrotaxis = glm::vec3(0, 1, 0);
    glm::mat4 xrot = glm::mat4(rotation_mat3(xrotaxis, -deltaX * sensitivity));
    glm::vec3 yrotaxis = glm::cross(glm::vec3(camera.getUp()), glm::vec3(camera.getLook()));
    glm::mat4 yrot = glm::mat4(rotation_mat3(yrotaxis, deltaY * sensitivity));

    glm::mat4 totrot = xrot * yrot;
    glm::vec4 newLook = totrot * camera.getLook();
    glm::vec4 newUp = totrot * camera.getUp();

    camera.setLook(newLook);
    camera.setUp(newUp);
}

glm::mat3 Realtime::rotation_mat3(glm::vec3 axis, float angle) {
    axis = glm::normalize(axis);
    float cost = cos(angle);
    float sint = sin(angle);
    float ux = axis[0];
    float uy = axis[1];
    float uz = axis[2];

    glm::mat3 r = glm::mat3(cost+pow(ux,2)*(1-cost), ux*uy*(1-cost)+uz*sint, ux*uz*(1-cost)-uy*sint,
                            ux*uy*(1-cost)-uz*sint, cost+pow(uy,2)*(1-cost), uy*uz*(1-cost)+ux*sint,
                            ux*uz*(1-cost)+uy*sint, uy*uz*(1-cost)-ux*sint, cost+pow(uz,2)*(1-cost));
    return r;
}
