#include "realtime.h"
#include "../src/utils/debug.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "./utils/sceneparser.h"
#include "./utils/shaderloader.h"

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

    // building data
    std::vector<float> allBuildingData; // building vertex data
    const int numBuildings = 20;

    for (int i = 0; i < numBuildings; i++) {
        Building* building = new Building();
        building->updateParams(); // generates pseudorandomized building
        std::vector<float> buildingData = building->generateShape();

        // position buildings in a curve // Why are we doing this? - remove
        float angle = (i / static_cast<float>(numBuildings)) * glm::pi<float>();
        float radius = 20.0f;

        // position the buildings
        glm::vec3 position(
            radius * cos(angle),
            0,
            radius * sin(angle) - 10.0f
        );

        // get position from vertices
        for (size_t j = 0; j < buildingData.size(); j += 6) {
            glm::vec4 pos(buildingData[j], buildingData[j+1], buildingData[j+2], 1.0f);
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
            pos = transform * pos;

            // add posoition and color
            allBuildingData.push_back(pos.x);
            allBuildingData.push_back(pos.y);
            allBuildingData.push_back(pos.z);
            allBuildingData.push_back(buildingData[j+3]);
            allBuildingData.push_back(buildingData[j+4]);
            allBuildingData.push_back(buildingData[j+5]);
        }

        delete building;
    }

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

    // camera data
    m_renderData.cameraData.pos = glm::vec4(0, 10, 30, 1);  // Moved back and up to see all buildings
    m_renderData.cameraData.look = glm::vec4(0, 0, -1, 0);
    m_renderData.cameraData.up = glm::vec4(0, 1, 0, 0);
    m_renderData.cameraData.heightAngle = 45.0f * M_PI / 180.0f;

    // transformation matrices
    m_camera = Camera(m_renderData.cameraData,
                      size().width() * m_devicePixelRatio,
                      size().height() * m_devicePixelRatio);

    m_view = m_camera.getViewMatrix();
    m_proj = getProjectionMatrix(0.1f, 100.0f);

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
    glBindVertexArray(m_vao);
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

    glm::vec3 look = glm::vec3(m_camera.getLook());
    glm::vec3 up = glm::vec3(m_camera.getUp());

    // x rotation around world up vector
    if (deltaX != 0) {
        glm::vec3 worldSpaceVector = glm::vec3(0, 1, 0);
        glm::mat3 xRotation = m_camera.rotationMatrix(worldSpaceVector, deltaX * rotationSpeed);
        look = xRotation * look;
        up = xRotation * up;
    }

    // y rotation around right vector
    if (deltaY != 0) {
        glm::vec3 right = glm::normalize(glm::cross(look, up));
        glm::mat3 yRotation = m_camera.rotationMatrix(right, deltaY * rotationSpeed);
        look = yRotation * look;
        up = yRotation * up;
    }

    m_camera.setLook(glm::vec4(look, 0.0f));
    m_camera.setUp(glm::vec4(up, 0.0f));
    m_view = m_camera.getViewMatrix();
}


void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    glm::vec4 pos = m_camera.getPos();
    glm::vec3 look = - m_camera.getLook();
    glm::vec3 up = m_camera.getUp();

    glm::vec3 lookVector = glm::normalize(-look);
    glm::vec3 rightVector = glm::cross(glm::normalize(look), glm::normalize(up));


    // Use deltaTime and m_keyMap here to move around

    glm::vec3 shift = glm::vec3(0.0f);
    const float moveSpeed = 5.0f;
    float moveAmount = moveSpeed * deltaTime;

    // W: Translates the camera in the direction of the look vector
    if (m_keyMap[Qt::Key_W]) {
        shift += lookVector * moveAmount;
    }

    // S: Translates the camera in the opposite direction of the look vector
    if (m_keyMap[Qt::Key_S]) {
        shift -= lookVector * moveAmount;
    }

    // A: Translates the camera in the left direction, perpendicular to the look and up vectors
    if (m_keyMap[Qt::Key_A]) {
        shift += rightVector * moveAmount;
    }

    // D: Translates the camera in the right direction, also perpendicular to the look and up vectors. This movement should be opposite to that of pressing A
    if (m_keyMap[Qt::Key_D]) {
        shift -= rightVector * moveAmount;
    }

    // Space: Translates the camera along the world space vector (0, 1, 0)
    if (m_keyMap[Qt::Key_Space]) {
        shift += glm::vec3(0, 1, 0) * moveAmount;
    }

    // Ctrl: Translates the camera along the world space vector (0, -1, 0)
    if (m_keyMap[Qt::Key_Control]) {
        shift -= glm::vec3(0, 1, 0) * moveAmount;
    }

    if (glm::length(shift) > 0) {
        m_camera.setPos(pos + glm::vec4(shift, 0.0f));

        m_view = m_camera.getViewMatrix();
        update();
    }
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
