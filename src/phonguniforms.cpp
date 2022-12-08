#include <realtime.h>

void Realtime::passGlobalData() {
    glUniform1f(glGetUniformLocation(shader, "ka"), renderData.globalData.ka);
    glUniform1f(glGetUniformLocation(shader, "kd"), renderData.globalData.kd);
    glUniform1f(glGetUniformLocation(shader, "ks"), renderData.globalData.ks);
}

void Realtime::passCameraData() {
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "proj"), 1, GL_FALSE, &proj[0][0]);
    glm::vec3 camerapos = glm::vec3(inverse(view) * glm::vec4(0.f,0.f,0.f,1.f));
    glUniform3fv(glGetUniformLocation(shader, "camerapos"), 1, &camerapos[0]);
}

void Realtime::passShapeData(RenderShapeData& shape) {
    glUniformMatrix4fv(glGetUniformLocation(shader, "ctm"), 1, GL_FALSE, &shape.ctm[0][0]);
    glUniformMatrix3fv(glGetUniformLocation(shader, "invtransctm"), 1, GL_FALSE,
                       &glm::transpose(glm::inverse(glm::mat3(shape.ctm)))[0][0]);
    glUniform1f(glGetUniformLocation(shader, "n"), shape.primitive.material.shininess);
    glUniform4fv(glGetUniformLocation(shader, "oa"), 1, &shape.primitive.material.cAmbient[0]);
    glUniform4fv(glGetUniformLocation(shader, "od"), 1, &shape.primitive.material.cDiffuse[0]);
    glUniform4fv(glGetUniformLocation(shader, "os"), 1, &shape.primitive.material.cSpecular[0]);
}

void Realtime::passLightData(SceneLightData& light, int lightNum) {
    std::string ltypestr = "lights[" + std::to_string(lightNum) + "].type";
    std::string ldirstr = "lights[" + std::to_string(lightNum) + "].dir";
    std::string lposstr = "lights[" + std::to_string(lightNum) + "].pos";
    std::string lcolorstr = "lights[" + std::to_string(lightNum) + "].color";
    std::string lfuncstr = "lights[" + std::to_string(lightNum) + "].function";
    std::string langlestr = "lights[" + std::to_string(lightNum) + "].angle";
    std::string lpenstr = "lights[" + std::to_string(lightNum) + "].penumbra";
    switch(light.type) {
    case LightType::LIGHT_DIRECTIONAL:
        glUniform1i(glGetUniformLocation(shader, ltypestr.c_str()), 0);
        break;
    case LightType::LIGHT_POINT:
        glUniform1i(glGetUniformLocation(shader, ltypestr.c_str()), 1);
        break;
    case LightType::LIGHT_SPOT:
        glUniform1i(glGetUniformLocation(shader, ltypestr.c_str()), 2);
        break;
    default:
        glUniform1i(glGetUniformLocation(shader, ltypestr.c_str()), -1);
        break;
    }
    glUniform3fv(glGetUniformLocation(shader, ldirstr.c_str()), 1, &light.dir[0]);
    glUniform3fv(glGetUniformLocation(shader, lposstr.c_str()), 1, &light.pos[0]);
    glUniform4fv(glGetUniformLocation(shader, lcolorstr.c_str()), 1, &light.color[0]);
    glUniform3fv(glGetUniformLocation(shader, lfuncstr.c_str()), 1, &light.function[0]);
    glUniform1f(glGetUniformLocation(shader, lpenstr.c_str()), light.penumbra);
    glUniform1f(glGetUniformLocation(shader, langlestr.c_str()), light.angle);
}

void Realtime::clearShaderUniforms() {
    glUseProgram(shader);
    glUniform1f(glGetUniformLocation(shader, "ka"), 0);
    glUniform1f(glGetUniformLocation(shader, "kd"), 0);
    glUniform1f(glGetUniformLocation(shader, "ks"), 0);

    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &glm::mat4(0.f)[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "proj"), 1, GL_FALSE, &glm::mat4(0.f)[0][0]);
    glUniform3fv(glGetUniformLocation(shader, "camerapos"), 1, &glm::vec3(0.f)[0]);

    glUniformMatrix4fv(glGetUniformLocation(shader, "ctm"), 1, GL_FALSE, &glm::mat4(0.f)[0][0]);
    glUniformMatrix3fv(glGetUniformLocation(shader, "invtransctm"), 1, GL_FALSE, &glm::mat3(0)[0][0]);
    glUniform1f(glGetUniformLocation(shader, "n"), 0);
    glUniform4fv(glGetUniformLocation(shader, "oa"), 1, &glm::vec4(0.f)[0]);
    glUniform4fv(glGetUniformLocation(shader, "od"), 1, &glm::vec4(0.f)[0]);
    glUniform4fv(glGetUniformLocation(shader, "os"), 1, &glm::vec4(0.f)[0]);

    for (int i = 0; i < 8; ++i) {
        std::string ltypestr = "lights[" + std::to_string(i) + "].type";
        std::string ldirstr = "lights[" + std::to_string(i) + "].dir";
        std::string lcolorstr = "lights[" + std::to_string(i) + "].color";
        glUniform1f(glGetUniformLocation(shader, ltypestr.c_str()), 0.f);
        glUniform3fv(glGetUniformLocation(shader, ldirstr.c_str()), 1, &glm::vec3(0.f)[0]);
        glUniform4fv(glGetUniformLocation(shader, lcolorstr.c_str()), 1, &glm::vec4(0.f)[0]);
    }
    glUseProgram(0);
}
