#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readXML();
    if (!success) {
        return false;
    }

    // TODO: Use your code from Lab 5 here.

    renderData.globalData = fileReader.getGlobalData();
    renderData.lights = fileReader.getLights();
    renderData.cameraData = fileReader.getCameraData();

    renderData.shapes.clear();
    SceneNode* root = fileReader.getRootNode();
    std::vector<glm::mat4> ctms;

    glm::mat4 ctm = glm::mat4(1.f);
    traverseTree(root, renderData, &ctm);

    return true;
}

void SceneParser::traverseTree(SceneNode* node,
                               RenderData& renderData,
                               glm::mat4* parentCTM) {

    glm::mat4 ctm = *parentCTM;
    int numTransformations = node->transformations.size();
    for (int i = 0; i < numTransformations; ++i) {
        applyTransform(ctm, node->transformations[i]);
    }

    for (auto& primitive : node->primitives) {
        RenderShapeData rsd;
        rsd.primitive = *primitive;
        rsd.ctm = ctm;
        renderData.shapes.push_back(rsd);
    }

    for (auto& child : node->children) {
        traverseTree(child, renderData, &ctm);
    }
}

void SceneParser::applyTransform(glm::mat4& ctm, SceneTransformation* newTransform) {
    switch (newTransform->type) {
    case TransformationType::TRANSFORMATION_TRANSLATE:
        ctm = ctm * translate(newTransform->translate);
        break;
    case TransformationType::TRANSFORMATION_SCALE:
        ctm = ctm * scale(newTransform->scale);
        break;
    case TransformationType::TRANSFORMATION_ROTATE:
        ctm = ctm * rotate(newTransform->angle, newTransform->rotate);
        break;
    case TransformationType::TRANSFORMATION_MATRIX:
        ctm = ctm * newTransform->matrix;
        break;
    default:
        break;
    }
}
