#include "camera.h"
#include "settings.h"

Camera::Camera(SceneCameraData cameraData, int width, int height) {
    this->m_cameraData = cameraData;
    this->width = width;
    this->height = height;
}

glm::mat4 Camera::getViewMatrix() {
    glm::vec4 pos = m_cameraData.pos;
    glm::vec4 look = m_cameraData.look;
    glm::vec4 up = m_cameraData.up;

    glm::vec3 w = glm::normalize(-look);
    glm::vec3 v = glm::normalize(glm::vec3(up) - (glm::dot(glm::vec3(up), w))*w);
    glm::vec3 u = glm::cross(v, w);

    glm::mat4 M_t = glm::mat4({
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        -pos.x, -pos.y, -pos.z, 1.0f,
    });

    glm::mat4 M_r = glm::mat4({
        u.x, v.x, w.x, 0.0f,
        u.y, v.y, w.y, 0.0f,
        u.z, v.z, w.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    });

    return M_r * M_t;
}

glm::mat4 Camera::getProjectionMatrix() {
    // lecture projection matrix was not working (see below), so used this: https://www.songho.ca/opengl/gl_projectionmatrix.html
    float aspectRatio = this->getAspectRatio();
    float near = settings.nearPlane;
    float far = settings.farPlane;

    float theta_h = m_cameraData.heightAngle;

    float yScale = 1.0f / std::tan(theta_h / 2.0f);
    float xScale = yScale / aspectRatio;

    glm::mat4 M = glm::mat4(0.0f);

    M[0][0] = xScale;
    M[1][1] = yScale;
    M[2][2] = -(far + near) / (far - near);
    M[2][3] = -1.0f;
    M[3][2] = -(2.0f * far * near) / (far - near);

    return M;
}

float Camera::getAspectRatio() {
    return static_cast<float>(this->width) / static_cast<float>(this->height);
}

float Camera::getHeightAngle() {
    return m_cameraData.heightAngle;
}

float Camera::getFocalLength() {
    return (m_cameraData.focalLength != 0) ? m_cameraData.focalLength : 1.0f;
}

float Camera::getAperture() {
    return (m_cameraData.aperture != 0) ? m_cameraData.aperture : 3.0f;
}

void Camera::updateScreenSize(int width, int height) {
    this->width = width;
    this->height = height;
}

void Camera::updateCameraData(SceneCameraData cameraData) {
    this->m_cameraData = cameraData;
}




glm::mat3 Camera::rotationMatrix(const glm::vec3& axis, float angle) {
    glm::vec3 normAxis = glm::normalize(axis);
    float cosTheta = cos(angle);
    float sinTheta = sin(angle);

    return glm::mat3(
        cosTheta + normAxis.x * normAxis.x * (1 - cosTheta),
        normAxis.x * normAxis.y * (1 - cosTheta) - normAxis.z * sinTheta,
        normAxis.x * normAxis.z * (1 - cosTheta) + normAxis.y * sinTheta,

        normAxis.y * normAxis.x * (1 - cosTheta) + normAxis.z * sinTheta,
        cosTheta + normAxis.y * normAxis.y * (1 - cosTheta),
        normAxis.y * normAxis.z * (1 - cosTheta) - normAxis.x * sinTheta,

        normAxis.z * normAxis.x * (1 - cosTheta) - normAxis.y * sinTheta,
        normAxis.z * normAxis.y * (1 - cosTheta) + normAxis.x * sinTheta,
        cosTheta + normAxis.z * normAxis.z * (1 - cosTheta)
        );
}



// float Camera::getAspectRatio() const {
//     // Optional TODO: implement the getter or make your own design
//     float focalLength = this->getFocalLength();
//     float height = 2 * std::tan(getHeightAngle()) * focalLength;
//     float width = 2 * std::tan(getAperture()) * focalLength;
//     return width/height;

// }

void Camera::setPos(const glm::vec4& newPos) {
    m_cameraData.pos = newPos;
}

void Camera::setLook(const glm::vec4& newLook) {
    m_cameraData.look = newLook;
}

void Camera::setUp(const glm::vec4& newUp) {
    m_cameraData.up = newUp;
}

glm::vec4 Camera::getLook() const {
    return m_cameraData.look;
}

glm::vec4 Camera::getUp() const {
    return m_cameraData.up;
}

glm::vec4 Camera::getPos() const {
    return m_cameraData.pos;
}

float Camera::getHeightAngle() const {
    // Optional TODO: implement the getter or make your own design
    return this->renderData.cameraData.heightAngle;
}

float Camera::getFocalLength() const {
    // Optional TODO: implement the getter or make your own design

    return (this->renderData.cameraData.focalLength != 0) ? this->renderData.cameraData.focalLength : 1.0f;
}

float Camera::getAperture() const {
    // Optional TODO: implement the getter or make your own design
    return this->renderData.cameraData.aperture;
}




