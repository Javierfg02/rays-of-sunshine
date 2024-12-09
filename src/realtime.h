#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include "./utils/camera.h"
#include "./utils/shadermanager.h"
#include "shapes/building.h"
#include "./features/bezier.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    GLuint m_vertexCount;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;
    float m_aspect_ratio;

    // RenderData
    RenderData m_renderData;

    // Camera
    Camera m_camera;
    float m_verticalVelocity = 0.0f;

    // Bezier
    Bezier m_walkCurve;
    float m_walkingTime = 0.0f;
    float m_stopTime = 0.0f;
    bool m_isWalking = false;
    bool m_isJumping = false;
    float t = 0;

    // Shader Manager
    ShaderManager& m_shaderManager = ShaderManager::getInstance();

    // VBO/VAO
    GLuint m_vbo;
    GLuint m_vao;

    // transforamtion matrices
    glm::mat4 m_model = glm::mat4(1);
    glm::mat4 m_view  = glm::mat4(1);
    glm::mat4 m_proj  = glm::mat4(1);
    glm::mat4 mvp_matrix;

    // Shapes
    Building* m_building;
    std::vector<RenderShapeData> m_shapes;
    void rotateCamera(float deltaX, float deltaY);

    // FBO
    void makeFBO();
    void paintTexture(GLuint texture);

    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;
    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;

    GLuint m_defaultFBO;
    int m_fbo_width;
    int m_fbo_height;
    int m_screen_width;
    int m_screen_height;

};
