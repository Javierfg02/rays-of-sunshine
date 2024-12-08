#include "realtime.h"
#include "../src/utils/debug.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "./utils/sceneparser.h"
#include "./utils/shaderloader.h"
#include "./features/buildinggenerator.h"

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
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();
    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initialize GL
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(settings.backgroundColor[0], settings.backgroundColor[1], settings.backgroundColor[2], settings.backgroundColor[3]);

    // shader manager
    m_shaderManager.initializeShaders();

    // generate buildings
    std::vector<float> allBuildingData; // building vertex data
    BuildingGenerator* buildingGenerator = new BuildingGenerator();
    allBuildingData = buildingGenerator->initializeBuildings();

    // set camera to road
    glm::vec3 roadPos = buildingGenerator->getRandomRoadPosition();
    // Position camera at start of road, looking down it
    m_renderData.cameraData.pos = glm::vec4(roadPos.x + 10.0f, 1.0f, roadPos.z, 1);  // On the road
    m_renderData.cameraData.look = glm::vec4(1, 0, 0, 0); // look down the road (along x-axis)
    m_renderData.cameraData.up = glm::vec4(0, 1, 0, 0);
    m_renderData.cameraData.heightAngle = 45.0f * M_PI / 180.0f;

    m_vertexCount = allBuildingData.size() / 6;

    // vao and vbo setup
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, allBuildingData.size() * sizeof(float), allBuildingData.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glEnableVertexAttribArray(0); // position
    glEnableVertexAttribArray(1); // color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());

    // transformation matrices
    m_camera = Camera(m_renderData.cameraData,
                      size().width() * m_devicePixelRatio,
                      size().height() * m_devicePixelRatio);

    m_view = m_camera.getViewMatrix();
    m_proj = getProjectionMatrix(0.1f, 100.0f);  // much farther far plane to see more of the city

    // Since we've baked the transforms into the vertices, we only need an identity matrix
    RenderShapeData buildingShape;
    buildingShape.ctm = glm::mat4(1.0f);
    m_shapes.push_back(buildingShape);
}

void Realtime::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the building shader
    GLuint shader = m_shaderManager.getShader(ShaderManager::ShaderType::BUILDING);
    glUseProgram(shader);

    // send matrices to shader
    glm::mat4 model = m_shapes[0].ctm;
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewMatrix"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projMatrix"), 1, GL_FALSE, &m_proj[0][0]);

    // draw building
    glBindVertexArray(m_vao); // vao will keep a reference to the vbo, so only need to bind vao
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    m_aspect_ratio = static_cast<float>(w) / static_cast<float>(h);
    m_proj = getProjectionMatrix(0.1f, 100.0f);
}

glm::mat4 Realtime::getProjectionMatrix(float near, float far) {
    // source is https://www.songho.ca/opengl/gl_projectionmatrix.html because lecture matrix did not work (see implementation above)

    float thetaH = m_camera.getHeightAngle();

    float scaleY = 1.0f / std::tan(thetaH / 2.0f);
    float scaleX = scaleY / m_aspect_ratio;

    glm::mat4 proj = glm::mat4(0.0f);

    proj[0][0] = scaleX;
    proj[1][1] = scaleY;
    proj[2][2] = -(far + near) / (far - near);
    proj[2][3] = -1.0f;
    proj[3][2] = -(2.0f * far * near) / (far - near);

    return proj;
}
void Realtime::sceneChanged() {
    SceneParser::parse(settings.sceneFilePath, this->m_renderData);

    // init camera only if scene is changed
    this->m_camera = Camera(m_renderData.cameraData,
                            size().width() * m_devicePixelRatio,
                            size().height() * m_devicePixelRatio);
    m_proj = getProjectionMatrix(settings.nearPlane, settings.farPlane);
    m_view = m_camera.getViewMatrix();

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {

    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

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
        rotateCamera(deltaX, deltaY);

        update(); // asks for a PaintGL() call to occur
    }
}


void Realtime::rotateCamera(float deltaX, float deltaY) {
    const float rotationSpeed = 0.002f;

    // eetrieve the current look vector and up vector
    glm::vec3 look = glm::vec3(m_camera.getLook());
    glm::vec3 up = glm::vec3(m_camera.getUp());

    // compute the current pitch (angle above/below horizontal)
    glm::vec3 forwardXZ = glm::normalize(glm::vec3(look.x, 0.0f, look.z));
    float currentPitch = glm::degrees(std::asin(glm::clamp(look.y, -1.0f, 1.0f)));

    // clamp the pitch change
    float newPitch = glm::clamp(currentPitch - deltaY * rotationSpeed * 90.0f, -20.0f, 30.0f);

    // update the look vector with the clamped pitch
    float pitchRad = glm::radians(newPitch);
    look.y = std::sin(pitchRad);
    look.x = forwardXZ.x * std::cos(pitchRad);
    look.z = forwardXZ.z * std::cos(pitchRad);

    static float yawAngle = 90.0f; // yaw angle in degrees
    float deltaYaw = deltaX * rotationSpeed * 90.0f; // convert deltaX to yaw angle change
    yawAngle = glm::clamp(yawAngle + deltaYaw, 0.0f, 180.0f); // clamp yaw to [-90, 90] degrees

    if (deltaX != 0) {
        glm::vec3 worldUp(0, 1, 0);
        glm::mat3 yawRotation = m_camera.rotationMatrix(worldUp, glm::radians(yawAngle));
        look = glm::normalize(yawRotation * glm::vec3(0.0f, look.y, -1.0f));
        up = glm::normalize(yawRotation * up);
    }

    // update the camera with the adjusted vectors
    m_camera.setLook(glm::vec4(look, 0.0f));
    m_camera.setUp(glm::vec4(up, 0.0f));
    m_view = m_camera.getViewMatrix();
}


void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f; // Convert milliseconds to seconds
    m_elapsedTimer.restart();

    glm::vec4 currentPos = m_camera.getPos();

    // gravity: update vertical velocity and apply to position
    m_verticalVelocity -= settings.gravity * deltaTime;
    currentPos.y += m_verticalVelocity * deltaTime;

    // check for ground collision
    if (currentPos.y <= settings.min_height) {
        currentPos.y = settings.min_height;
        m_verticalVelocity = 0.0f; // Reset vertical velocity on the ground
    }

    // movement adjustments
    glm::vec3 shift = glm::vec3(0.0f);
    float moveSpeed = settings.moveSpeed * deltaTime;

    // move forward (W)
    if (m_keyMap[Qt::Key_W]) {
        if (currentPos.x >= 85.0f) {
            currentPos.x = 84.9f;
        }
        shift += glm::vec3(1.0f, 0.0f, 0.0f) * moveSpeed; // Fixed forward movement along X
    }

    // move backward (S)
    if (m_keyMap[Qt::Key_S]) {
        if (currentPos.x <= 0.0f) {
            currentPos.x = 0.1f;
        }
        shift -= glm::vec3(1.0f, 0.0f, 0.0f) * moveSpeed; // Fixed backward movement along X
    }

    // jump
    if (m_keyMap[Qt::Key_Space] && currentPos.y <= settings.min_height) {
        m_verticalVelocity = settings.jump_force; // Apply jump impulse
    }

    // spring
    if(m_keyMap[Qt::Key_Shift]) {
        settings.moveSpeed = 5.0f;
    } else {
        settings.moveSpeed = 2.5f;
    }

    // update position and camera
    currentPos += glm::vec4(shift, 0.0f);
    m_camera.setPos(currentPos);
    m_view = m_camera.getViewMatrix();

    // call paintGL
    update();
}


// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
