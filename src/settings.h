#ifndef SETTINGS_H
#define SETTINGS_H

#include <glm/glm.hpp>
#include <string>

struct Settings {
    // metadata
    std::string sceneFilePath;
    int shapeParameter1 = 1;
    int shapeParameter2 = 1;
    float nearPlane = 1;
    float farPlane = 100.f;
    std::string inputFile = "cube";
    std::string outputFile = "cube";

    // scene data
    int width = 0;
    int height = 0;
    glm::vec4 backgroundColor = glm::vec4(0.1f, 0.2f, 0.3f, 1.0f);

    // buildings
    int numBuildings = 20;
    float buildingSpacing = 1.0f;

    // building
    float buildingMaxWidth = 5.f;
    float windowWidth = 0.2f;
    float windowHeight = 0.3f;
    glm::vec3 windowColor = glm::vec3(255.f/255.f, 197.f/255.f, 143.f/255.f);
    glm::vec3 wallColor = glm::vec3(0.4f, 0.4f, 0.4f);

    // gravity
    float moveSpeed = 1.5f;
    float gravity = 9.81;
    float min_height = 2.0f;
    float jump_force = 3.5f;

    // camera
    float cameraPitchMin = -25.0f;
    float cameraPitchMax = 30.0f;
    float cameraYawMin = 30.0f;
    float cameraYawMax = 150.0f;

    // bezier
    float baseHeight = 2.0f;
    float stepHeight = 0.5f;
    float stopDuration = 1.0f;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
