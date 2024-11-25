#ifndef CAMERA_H
#define CAMERA_H

#include "sceneparser.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

class Camera {

public:
    Camera() = default;
    Camera(SceneCameraData cameraData, int width, int height);

    // getters
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    float getAspectRatio();
    float getHeightAngle();
    float getFocalLength();
    float getAperture();

    // update methods
    void updateCameraData(SceneCameraData cameraData);
    void updateScreenSize(int width, int height);

private:
    SceneCameraData m_cameraData;

    int width;
    int height;
};

#endif // CAMERA_H
