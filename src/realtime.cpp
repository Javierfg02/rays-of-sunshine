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
    m_defaultFBO = 2;
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
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
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
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
    m_proj = m_camera.getProjectionMatrix();  // much farther far plane to see more of the city

    // Since we've baked the transforms into the vertices, we only need an identity matrix
    RenderShapeData buildingShape;
    buildingShape.ctm = glm::mat4(1.0f);
    m_shapes.push_back(buildingShape);

    // set up shaders
    GLuint dof_shader = m_shaderManager.getShader(ShaderManager::ShaderType::DEPTH_OF_FIELD);
    glUseProgram(dof_shader);

    GLint colorTextureLoc = glGetUniformLocation(dof_shader, "colorTexture");
    glUniform1i(colorTextureLoc, 0);

    GLint depthTextureLoc = glGetUniformLocation(dof_shader, "depthTexture");
    glUniform1i(depthTextureLoc, 1);

    glUseProgram(0);

    setupFullscreenQuad();

    makeFBO();
}

void Realtime::paintGL() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_scene_fbo);
    glViewport(0, 0, m_fbo_width, m_fbo_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the building shader
    GLuint building_shader = m_shaderManager.getShader(ShaderManager::ShaderType::BUILDING);
    glUseProgram(building_shader);

    // send matrices to shader
    glm::mat4 model = m_shapes[0].ctm;
    glUniformMatrix4fv(glGetUniformLocation(building_shader, "modelMatrix"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(building_shader, "viewMatrix"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(building_shader, "projMatrix"), 1, GL_FALSE, &m_proj[0][0]);

    // draw building
    glBindVertexArray(m_vao); // vao will keep a reference to the vbo, so only need to bind vao
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, m_screen_width, m_screen_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // PAINT TEXTURE -----------------------------
    paintTexture();
    // paintTexture(depthTexture);
}

void Realtime::resizeGL(int w, int h) {
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    m_aspect_ratio = static_cast<float>(w) / static_cast<float>(h);
    m_proj = m_camera.getProjectionMatrix();
}

void Realtime::setupFullscreenQuad() {
    std::vector<GLfloat> fullscreen_quad_data =
        { //    POSITIONS    //    UV COORDS
            -1.0f, 1.0f,  0.0f,   0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
            1.0f,  -1.0f, 0.0f,   1.0f, 0.0f,
            1.0f,  1.0f,  0.0f,   1.0f, 1.0f,
            -1.0f, 1.0f,  0.0f,   0.0f, 1.0f,
            1.0f,  -1.0f, 0.0f,   1.0f, 0.0f
        };

    // generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size() * sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    // define vao attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::renderFullscreenQuad() {
    glBindVertexArray(m_fullscreen_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void Realtime::makeFBO(){
    // GENERAL SCENE FBO -----------------------------
    colorTexture = generateTexture(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    depthTexture = generateTexture(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
    setupFramebuffer(m_scene_fbo, colorTexture, depthTexture);

    // HORIZONTAL BLUR FBO -----------------------------
    hblurTexture = generateTexture(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    setupFramebuffer(m_hblur_fbo, hblurTexture, 0);

    // VERTCICAL BLUR FBO -----------------------------
    vblurTexture = generateTexture(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    setupFramebuffer(m_vblur_fbo, vblurTexture, 0);
}

void Realtime::paintTexture(){
    // HORIZONTAL BLUR -----------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, m_hblur_fbo);
    glViewport(0, 0, m_screen_width, m_screen_height);

    GLuint hblur_shader = m_shaderManager.getShader(ShaderManager::ShaderType::HORIZONTAL_BLUR);
    glUseProgram(hblur_shader);

    GLint hblurTextureLoc = glGetUniformLocation(hblur_shader, "hblurTexture");
    glUniform1i(hblurTextureLoc, 2); // maybe move idk
    GLint uvChangeHLoc = glGetUniformLocation(hblur_shader, "uvChange");
    glUniform2f(uvChangeHLoc, 1.0 / m_fbo_width, 0.0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    renderFullscreenQuad();

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);

    // VERTICAL BLUR -----------------------------

    glBindFramebuffer(GL_FRAMEBUFFER, m_vblur_fbo);
    glViewport(0, 0, m_screen_width, m_screen_height);

    GLuint vblur_shader = m_shaderManager.getShader(ShaderManager::ShaderType::VERTICAL_BLUR);
    glUseProgram(vblur_shader);

    GLint vblurTextureLoc = glGetUniformLocation(vblur_shader, "vblurTexture");
    glUniform1i(vblurTextureLoc, 3); // maybe move
    GLint uvChangeVLoc = glGetUniformLocation(vblur_shader, "uvChange");
    glUniform2f(uvChangeVLoc, 0.0, 1.0 / m_fbo_height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hblurTexture);
    renderFullscreenQuad();

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);

    // DOF COMBINATION -----------------------------

    glBindFramebuffer(GL_FRAMEBUFFER, m_scene_fbo);
    glViewport(0, 0, m_screen_width, m_screen_height);

    GLuint dof_shader = m_shaderManager.getShader(ShaderManager::ShaderType::DEPTH_OF_FIELD);
    glUseProgram(dof_shader);

    // bind vao, draw, unbind vao
    // glBindVertexArray(m_fullscreen_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, vblurTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    renderFullscreenQuad();
    // glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    // glBindTexture(GL_TEXTURE_2D, 0);
    // glBindVertexArray(0);

    // glUseProgram(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

GLuint Realtime::generateTexture(GLint internalFormat, GLint format, GLenum type) {
    GLuint texture;
    glGenTextures(1, &texture);
    // state active texture? -- glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_fbo_width, m_fbo_height, 0, format, type, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

void Realtime::setupFramebuffer(GLuint &fbo, GLuint colorTexture, GLuint depthTexture) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    if (depthTexture) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Realtime::sceneChanged() {
    SceneParser::parse(settings.sceneFilePath, this->m_renderData);

    // init camera only if scene is changed
    this->m_camera = Camera(m_renderData.cameraData,
                            size().width() * m_devicePixelRatio,
                            size().height() * m_devicePixelRatio);
    m_proj = m_camera.getProjectionMatrix();
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
    static float pitchAngle = 0.0f;
    static float yawAngle = 90.0f;

    // Calculate next angles
    float nextPitch = pitchAngle - deltaY * rotationSpeed * 90.0f;
    float nextYaw = yawAngle + deltaX * rotationSpeed * 90.0f;

    // Update pitch if within limits
    if (nextPitch >= settings.cameraPitchMin && nextPitch <= settings.cameraPitchMax) {
        pitchAngle = nextPitch;
    }

    // Update yaw if within limits
    if (nextYaw >= settings.cameraYawMin && nextYaw <= settings.cameraYawMax) {
        yawAngle = nextYaw;
    }

    // Convert angles to radians for calculations
    float pitchRad = glm::radians(pitchAngle);
    float yawRad = glm::radians(yawAngle);

    // Calculate look vector from angles
    glm::vec3 look;
    look.x = cos(pitchRad) * sin(yawRad);
    look.y = sin(pitchRad);
    look.z = cos(pitchRad) * -cos(yawRad);

    // Calculate up vector
    glm::vec3 worldUp(0, 1, 0);
    glm::vec3 right = glm::normalize(glm::cross(look, worldUp));
    glm::vec3 up = glm::normalize(glm::cross(right, look));

    // Update camera
    m_camera.setLook(glm::vec4(glm::normalize(look), 0.0f));
    m_camera.setUp(glm::vec4(up, 0.0f));
    m_view = m_camera.getViewMatrix();
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f; // convert milliseconds to seconds always 0.016s elapsed ~= 60fps
    // std::cout << "deltaTime: " << deltaTime << std::endl;
    m_elapsedTimer.restart();

    glm::vec4 currentPos = m_camera.getPos();

    // // gravity: update vertical velocity and apply to position
    // if (m_isJumping) {
    //     m_verticalVelocity -= settings.gravity * deltaTime;
    //     currentPos.y += m_verticalVelocity * deltaTime;
    // }

    // // check for ground collision
    // if (currentPos.y <= settings.min_height) {
    //     currentPos.y = 0.0f;
    //     m_verticalVelocity = 0.0f; // Reset vertical velocity on the ground
    // }

    // movement adjustments
    float moveSpeed = settings.moveSpeed * deltaTime;

    // move forward (W)
    if (m_keyMap[Qt::Key_W] && currentPos.x <= 85.f) {
        this->m_walkingTime += deltaTime;
        this->t = this->m_walkingTime;
        // apply bezier curve to camera motion (currently just sinusoidal)
        currentPos.y = 1.25f * std::abs(std::cos(this->m_walkingTime * 4));
        currentPos.x += moveSpeed;
    } // move backward (S)
    else if (m_keyMap[Qt::Key_S] && currentPos.x >= 0.1f) {
        this->m_walkingTime += deltaTime;
        this->t = this->m_walkingTime;
        currentPos.y = 1.25f * std::abs(std::cos(this->m_walkingTime * 4));
        currentPos.x -= moveSpeed;
    } else {
        this->m_walkingTime = this->t;
    }

    std::cout << "currentPos.y: " << currentPos.y << std::endl;

    // jump
    if (m_keyMap[Qt::Key_Space] && currentPos.y <= settings.min_height) {
        m_isJumping = true;
        m_verticalVelocity = settings.jump_force; // Apply jump impulse
    } else {
        m_isJumping = false;
    }

    // spring
    if(m_keyMap[Qt::Key_Shift]) {
        settings.moveSpeed = 5.0f;
    } else {
        settings.moveSpeed = 2.5f;
    }

    // update position and camera
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
