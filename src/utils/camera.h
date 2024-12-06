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
    glm::vec4 getLook() const;
    glm::vec4 getUp() const;
    glm::vec4 getPos() const;

    void setPos(const glm::vec4& newPos);
    void setLook(const glm::vec4& newLook);
    void setUp(const glm::vec4& newUp);

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;

    glm::mat3 rotationMatrix(const glm::vec3& axis, float angle);

    RenderData renderData;

private:
    SceneCameraData m_cameraData;

    int width;
    int height;
};

#endif // CAMERA_H




// float Camera::getAspectRatio() const {
//     // Optional TODO: implement the getter or make your own design
//     float focalLength = this->getFocalLength();
//     float height = 2 * std::tan(getHeightAngle()) * focalLength;
//     float width = 2 * std::tan(getAperture()) * focalLength;
//     return width/height;

// }
