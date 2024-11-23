#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // TODO: Use your Lab 5 code here
    // Task 5: populate renderData with global data, and camera data;
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    // Task 6: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!
    SceneNode* root = fileReader.getRootNode();
    renderData.shapes.clear();

    glm::mat4 identityMatrix = glm::mat4(1.0f);
    renderDataDFS(root, identityMatrix, renderData);

    return true;
}

void SceneParser::renderDataDFS(SceneNode* node, glm::mat4& parentCTM, RenderData &renderData) {

    // base case: end of tree
    if (node == nullptr) {
        return;
    }

    glm::mat4 ctm = parentCTM; // build ctm throughout method

    // apply the transformations of the current node first
    for (const auto& transformation : node->transformations) {
        switch (transformation->type) {
        case TransformationType::TRANSFORMATION_TRANSLATE:
            ctm = glm::translate(ctm, transformation->translate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            ctm = glm::scale(ctm, transformation->scale);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            ctm = glm::rotate(ctm, transformation->angle, transformation->rotate);
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            ctm = ctm * transformation->matrix;  // Apply the custom matrix
            break;
        }
    }

    // now construct CTM for primitive
    for (const auto& primitive : node->primitives) {
        RenderShapeData shapeData;
        shapeData.primitive = *primitive;
        shapeData.ctm = ctm;
        renderData.shapes.push_back(shapeData); // add the primitive to the list
    }

    // construct the lights, too
    for (const auto& light : node->lights) {
        SceneLightData lightData;
        lightData.type = light->type;
        lightData.id = light->id;
        lightData.color = light->color;
        lightData.function = light->function;

        if (light->type == LightType::LIGHT_POINT || light->type == LightType::LIGHT_SPOT) {
            lightData.pos = ctm * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }

        if (light->type == LightType::LIGHT_SPOT || light->type == LightType::LIGHT_DIRECTIONAL) {
            lightData.dir = glm::normalize(ctm * light->dir);
        }

        if (light->type == LightType::LIGHT_SPOT) {
            lightData.penumbra = light->penumbra;
            lightData.angle = light->angle;
        }

        renderData.lights.push_back(lightData);
    }

    // Lastly, we do DFS (recursion through node's children)
    for (SceneNode* child : node->children) {
        renderDataDFS(child, ctm, renderData);
    }
}
