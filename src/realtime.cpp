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
#include "./utils/shadermanager.h"

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
    this->makeCurrent();
    m_devicePixelRatio = this->devicePixelRatio();
    m_defaultFBO = 2;
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // initialize GL
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
    srand(time(nullptr));
    BuildingGenerator* buildingGenerator = new BuildingGenerator();
    allBuildingData = buildingGenerator->initializeBuildings();

    // clear any existing lights
    m_renderData.lights.clear();

    // Add a bright directional light for testing
    // SceneLightData directionalLight;
    // directionalLight.id = 0;
    // directionalLight.type = LightType::LIGHT_DIRECTIONAL;
    // directionalLight.color = glm::vec4(2.0f, 2.0f, 2.0f, 2.0f);  // Bright white light
    // directionalLight.dir = glm::normalize(glm::vec4(0.3f, 0.3f, 0.3f, 0.0f));
    // directionalLight.function = glm::vec3(1.0f, 0.0f, 0.0f);
    // m_renderData.lights.push_back(directionalLight);

    // set default global illumination coefficients
    m_renderData.globalData.ka = settings.ka;  // low ambient for dark night scene
    m_renderData.globalData.kd = settings.kd;  // diffuse
    m_renderData.globalData.ks = settings.ks;  // specular

    // set camera to road
    glm::vec3 roadPos = buildingGenerator->getRandomRoadPosition();
    // position camera at start of road, looking down it
    m_renderData.cameraData.pos = glm::vec4(roadPos.x + 10.0f, 1.0f, roadPos.z, 1);  // On the road
    m_renderData.cameraData.look = glm::vec4(1, 0, 0, 0); // look down the road (along x-axis)
    m_renderData.cameraData.up = glm::vec4(0, 1, 0, 0);
    m_renderData.cameraData.heightAngle = 45.0f * M_PI / 180.0f;

    m_vertexCount = allBuildingData.size() / 9; // pos, color, normal

    std::cout << "Total vertex data size: " << allBuildingData.size() << std::endl;
    std::cout << "Calculated vertex count: " << m_vertexCount << std::endl;

    this->setUpArrays(allBuildingData, buildingGenerator);

    // transformation matrices
    m_camera = Camera(m_renderData.cameraData,
                      size().width() * m_devicePixelRatio,
                      size().height() * m_devicePixelRatio);

    m_view = m_camera.getViewMatrix();
    m_proj = m_camera.getProjectionMatrix();

    // Since we've baked the transforms into the vertices, we only need an identity matrix
    RenderShapeData buildingShape;
    buildingShape.ctm = glm::mat4(1.0f);
    m_shapes.push_back(buildingShape);

    // create dof texture uniforms
    glUseProgram(m_shaderManager.dof_shader);
    GLint colorTextureLoc = glGetUniformLocation(m_shaderManager.dof_shader, "colorTexture");
    glUniform1i(colorTextureLoc, 0);
    GLint depthTextureLoc = glGetUniformLocation(m_shaderManager.dof_shader, "depthTexture");
    glUniform1i(depthTextureLoc, 1);
    glUseProgram(0);

    setupFullscreenQuad();

    makeFBO();
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

void Realtime::setUpArrays(std::vector<float>& allBuildingData, BuildingGenerator* buildingGenerator) {

    // Buildings vao and vbo setup
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferData(GL_ARRAY_BUFFER, allBuildingData.size() * sizeof(float), allBuildingData.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glEnableVertexAttribArray(0); // position
    glEnableVertexAttribArray(1); // normal
    glEnableVertexAttribArray(2); // color

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); // normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float))); // color

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Generate road data
    std::vector<float> roadData = buildingGenerator->getRoadData();
    m_roadVertexCount = roadData.size() / 9; // 9 floats per vertex

    // Create and bind road VAO/VBO
    glGenBuffers(1, &m_road_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_road_vbo);
    glBufferData(GL_ARRAY_BUFFER, roadData.size() * sizeof(float), roadData.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_road_vao);
    glBindVertexArray(m_road_vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); // normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float))); // color

    // unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::renderFullscreenQuad() {
  glBindVertexArray(m_fullscreen_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void Realtime::paintGL() {
    // bind fbo
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_fbo_width, m_fbo_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the building shader
    glUseProgram(m_shaderManager.building_shader);

    // update spot light
    this->updateSpotLight();

    // set global uniforms
    this->setGlobalUniforms(m_shaderManager.building_shader);

    // draw buildings
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);

    // draw road - reapply uniforms to be safe
    this->setGlobalUniforms(m_shaderManager.building_shader);

    // Set specific material properties for road
    SceneMaterial roadMaterial = {
        glm::vec4(0.2f),       // ambient
        glm::vec4(0.5f),       // diffuse - darker than buildings
        glm::vec4(0.1f),       // less specular than buildings
        16.0f,                 // less shiny
        glm::vec4(1.0f)        // reflective
    };

    glUniform3fv(glGetUniformLocation(m_shaderManager.building_shader, "material.ambient"), 1, &roadMaterial.cAmbient[0]);
    glUniform3fv(glGetUniformLocation(m_shaderManager.building_shader, "material.diffuse"), 1, &roadMaterial.cDiffuse[0]);
    glUniform3fv(glGetUniformLocation(m_shaderManager.building_shader, "material.specular"), 1, &roadMaterial.cSpecular[0]);
    glUniform1f(glGetUniformLocation(m_shaderManager.building_shader, "material.shininess"), roadMaterial.shininess);

    // draw road
    glBindVertexArray(m_road_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_roadVertexCount);
    glBindVertexArray(0);
    std::cout << "Drawing road with " << m_roadVertexCount << " vertices" << std::endl;

    glUseProgram(0);

    // unbind fbo
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, m_screen_width, m_screen_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // paint textures
    paintTextures();
}

void Realtime::makeFBO(){
    // horizontal blur
    generateTexture(hblurTexture, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

    // vertical blur
    generateTexture(vblurTexture, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

    // general scene
    generateTexture(colorTexture, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    generateTexture(depthTexture, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);

    // GENERATE FBO -----------------------------
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete: " << status << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

void Realtime::paintTextures() {
    glUseProgram(m_shaderManager.dof_shader);

    // horizontal blur
    GLint uvChangeHLoc = glGetUniformLocation(m_shaderManager.dof_shader, "uvChangeH");
    glUniform2f(uvChangeHLoc, 1.0 / m_fbo_width, 0.0);

    // vertical blur
    GLint uvChangeVLoc = glGetUniformLocation(m_shaderManager.dof_shader, "uvChangeV");
    glUniform2f(uvChangeVLoc, 0.0, 1.0 / m_fbo_height);

    // dof combo
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    renderFullscreenQuad();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(0);
}

void Realtime::generateTexture(GLuint &texture, GLint internalFormat, GLint format, GLenum type) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_fbo_width, m_fbo_height, 0, format, type, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Realtime::updateSpotLight() {
    m_renderData.lights.clear();

    // Create a spotlight that follows the camera
    SceneLightData spotlight;
    spotlight.id = 0;
    spotlight.type = LightType::LIGHT_SPOT;
    spotlight.color = settings.spotColor;  // 50% brighter
    spotlight.pos = m_camera.getPos();  // light position is camera position - acts as a lantern
    spotlight.dir = m_camera.getLook();  // light direction is camera look direction
    spotlight.function = glm::vec3(0.0f, 0.05f, 0.02f); // attenuation function
    spotlight.angle = 0.5f;  // 30 degrees
    spotlight.penumbra = 0.2f;
    m_renderData.lights.push_back(spotlight);
}

void Realtime::setGlobalUniforms(GLuint shader) {
    // Send camera position for specular calculations
    glm::vec3 cameraPos = glm::vec3(m_camera.getPos());
    glUniform3fv(glGetUniformLocation(shader, "cameraPos"), 1, &cameraPos[0]);

    // Send matrices
    glm::mat4 model = m_shapes[0].ctm;
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewMatrix"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projMatrix"), 1, GL_FALSE, &m_proj[0][0]);

    // Send lighting coefficients
    glUniform1f(glGetUniformLocation(shader, "ka"), m_renderData.globalData.ka);
    glUniform1f(glGetUniformLocation(shader, "kd"), m_renderData.globalData.kd);
    glUniform1f(glGetUniformLocation(shader, "ks"), m_renderData.globalData.ks);

    // Send light data
    int numLights = m_renderData.lights.size();
    glUniform1i(glGetUniformLocation(shader, "numLights"), numLights);

    for (int i = 0; i < numLights; i++) {
        std::string prefix = "lights[" + std::to_string(i) + "].";
        SceneLightData light = m_renderData.lights[i];

        glUniform1i(glGetUniformLocation(shader, (prefix + "type").c_str()),
                    static_cast<int>(light.type));
        glUniform3fv(glGetUniformLocation(shader, (prefix + "color").c_str()),
                     1, &light.color[0]);
        glUniform3fv(glGetUniformLocation(shader, (prefix + "function").c_str()),
                     1, &light.function[0]);

        if (light.type != LightType::LIGHT_DIRECTIONAL) {
            glUniform4fv(glGetUniformLocation(shader, (prefix + "pos").c_str()),
                         1, &light.pos[0]);
        }

        if (light.type != LightType::LIGHT_POINT) {
            glUniform4fv(glGetUniformLocation(shader, (prefix + "dir").c_str()),
                         1, &light.dir[0]);
        }

        if (light.type == LightType::LIGHT_SPOT) {
            glUniform1f(glGetUniformLocation(shader, (prefix + "penumbra").c_str()),
                        light.penumbra);
            glUniform1f(glGetUniformLocation(shader, (prefix + "angle").c_str()),
                        light.angle);
        }
    }

    // set base material properties
    SceneMaterial material = {
        glm::vec4(0.2f), // ambient
        glm::vec4(0.7f), // diffuse
        glm::vec4(0.3f), // specular
        32.0f,           // shininess
        glm::vec4(1.1f)  // reflective
    };

    glUniform3fv(glGetUniformLocation(shader, "material.ambient"), 1, &material.cAmbient[0]);
    glUniform3fv(glGetUniformLocation(shader, "material.diffuse"), 1, &material.cDiffuse[0]);
    glUniform3fv(glGetUniformLocation(shader, "material.specular"), 1, &material.cSpecular[0]);
    glUniform1f(glGetUniformLocation(shader, "material.shininess"), material.shininess);
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
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    glm::vec4 currentPos = m_camera.getPos();
    float moveSpeed = settings.moveSpeed * deltaTime;
    bool isWalking = false;

    // handle forward/backward movement
    if (m_keyMap[Qt::Key_W] && currentPos.x <= 85.f) {
        currentPos.x += moveSpeed;
        isWalking = true;
    } else if (m_keyMap[Qt::Key_S] && currentPos.x >= 0.1f) {
        currentPos.x -= moveSpeed;
        isWalking = true;
    }

    // handle walking animation
    if (isWalking) {
        if (!m_wasWalking) {
            m_walkCycleTime = 0.0f;
            m_walkingCurve.resetCurve(currentPos.y);
        }

        float walkSpeed = settings.moveSpeed;
        m_walkCycleTime += deltaTime * walkSpeed;

        // get position from Bezier curve
        BezierPoint point = m_walkingCurve.bezierBasis(m_walkCycleTime);
        currentPos.y = point.y;

    } else if (m_wasWalking) {
        // smoothly return to base height
        currentPos.y = glm::mix(currentPos.y, settings.baseHeight, deltaTime * 5.0f);
    }

    // sprint
    if(m_keyMap[Qt::Key_Shift]) {
        settings.moveSpeed = settings.sprintSpeed;
    } else {
        settings.moveSpeed = 1.0f;
    }

    m_wasWalking = isWalking;
    m_camera.setPos(currentPos);
    m_view = m_camera.getViewMatrix();
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
