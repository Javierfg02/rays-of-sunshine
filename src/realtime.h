// // #pragma once
// // #include "shapes/building.h"

// // // Defined before including GLEW to suppress deprecation messages on macOS
// // #ifdef __APPLE__
// // #define GL_SILENCE_DEPRECATION
// // #endif

// // #include "./utils/camera.h"
// // #include "./utils/shadermanager.h"

// // #include <GL/glew.h>
// // #include <glm/glm.hpp>

// // #include <unordered_map>
// // #include <QElapsedTimer>
// // #include <QOpenGLWidget>
// // #include <QTime>
// // #include <QTimer>

// // class Realtime : public QOpenGLWidget
// // {
// // public:
// //     Realtime(QWidget *parent = nullptr);
// //     void finish();                                      // Called on program exit
// //     void sceneChanged();
// //     void settingsChanged();
// //     void saveViewportImage(std::string filePath);

// // public slots:
// //     void tick(QTimerEvent* event);                      // Called once per tick of m_timer

// // protected:
// //     void initializeGL() override;                       // Called once at the start of the program
// //     void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
// //     void resizeGL(int width, int height) override;      // Called when window size changes

// // private:
// //     Building m_building;
// //     void keyPressEvent(QKeyEvent *event) override;
// //     void keyReleaseEvent(QKeyEvent *event) override;
// //     void mousePressEvent(QMouseEvent *event) override;
// //     void mouseReleaseEvent(QMouseEvent *event) override;
// //     void mouseMoveEvent(QMouseEvent *event) override;
// //     void timerEvent(QTimerEvent *event) override;

// //     // Tick Related Variables
// //     int m_timer;                                        // Stores timer which attempts to run ~60 times per second
// //     QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

// //     // Input Related Variables
// //     bool m_mouseDown = false;                           // Stores state of left mouse button
// //     glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
// //     std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

// //     // Device Correction Variables
// //     double m_devicePixelRatio;

// //     // RenderData
// //     RenderData m_renderData;

// //     // Camera
// //     Camera m_camera;

// //     // Shader Manager
// //     ShaderManager& m_shaderManager = ShaderManager::getInstance();

// //     // VBO/VAO
// //     GLuint m_vbo;
// //     GLuint m_vao;

// //     // Camera
// //     glm::mat4 m_model = glm::mat4(1);
// //     glm::mat4 m_view  = glm::mat4(1);
// //     glm::mat4 m_proj  = glm::mat4(1);
// //     glm::mat4 mvp_matrix;

// //     std::vector<SceneLightData> m_lights;
// //     RenderData metaData;
// //     SceneGlobalData m_globalData;
// //     glm::mat4 getProjectionMatrix(float near, float far);

// //     int m_currParam1;
// //     int m_currParam2;
// //     float m_currNearPlane;
// //     float m_currFarPlane;
// //     float m_aspect_ratio;
// // };

// #pragma once

// // Defined before including GLEW to suppress deprecation messages on macOS
// #ifdef __APPLE__
// #define GL_SILENCE_DEPRECATION
// #endif
// #include <GL/glew.h>
// #include <glm/glm.hpp>

// #include <unordered_map>
// #include <QElapsedTimer>
// #include <QOpenGLWidget>
// #include <QTime>
// #include <QTimer>
// #include "utils/camera.h"
// #include "shapes/Cylinder.h"
// #include "shapes/Sphere.h"
// #include "shapes/Cone.h"
// #include "shapes/Cube.h"
// #include "shapes/building.h"



// class Realtime : public QOpenGLWidget
// {
// public:
//     Realtime(QWidget *parent = nullptr);
//     void finish();                                      // Called on program exit
//     void sceneChanged();
//     void settingsChanged();
//     void saveViewportImage(std::string filePath);

// public slots:
//     void tick(QTimerEvent* event);                      // Called once per tick of m_timer

// protected:
//     void initializeGL() override;                       // Called once at the start of the program
//     void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
//     void resizeGL(int width, int height) override;      // Called when window size changes

// private:
//     void keyPressEvent(QKeyEvent *event) override;
//     void keyReleaseEvent(QKeyEvent *event) override;
//     void mousePressEvent(QMouseEvent *event) override;
//     void mouseReleaseEvent(QMouseEvent *event) override;
//     void mouseMoveEvent(QMouseEvent *event) override;
//     void timerEvent(QTimerEvent *event) override;
//     glm::mat4 getProjectionMatrix(float near, float far);

//     // Tick Related Variables
//     int m_timer;                                        // Stores timer which attempts to run ~60 times per second
//     QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

//     // Input Related Variables
//     bool m_mouseDown = false;                           // Stores state of left mouse button
//     glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
//     std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

//     // Device Correction Variables
//     double m_devicePixelRatio;

//     // ADDED FROM LAB
//     GLuint m_shader;     // Stores id of shader program
//     GLuint m_sphere_vbo; // Stores id of vbo
//     GLuint m_sphere_vao; // Stores id of vao
//     std::vector<float> m_sphereData;

//     glm::mat4 m_model = glm::mat4(1);
//     glm::mat4 m_view  = glm::mat4(1);
//     glm::mat4 m_proj  = glm::mat4(1);
//     glm::mat4 mvp_matrix;

//     int m_vertexCount;

//     std::vector<SceneLightData> m_lights;
//     RenderData metaData;
//     SceneGlobalData m_globalData;
//     float m_aspect_ratio;


//     glm::vec4 m_lightPos; // The world-space position of the point light


//     Camera m_camera;

//     Sphere* m_sphere;
//     Cube* m_cube;
//     Cylinder* m_cylinder;
//     Cone* m_cone;
//     Building* m_building;
//     int m_currParam1;
//     int m_currParam2;
//     float m_currNearPlane;
//     float m_currFarPlane;

//     float m_ka;
//     float m_kd;
//     float m_ks;
//     float m_shininess;

//     GLuint m_vbo;
//     GLuint m_vao;
//     QPoint m_prevMousePos;
//     float  m_angleX;
//     float  m_angleY;
//     float  m_zoom;

//     struct shapeBuffers {
//         GLuint vao;
//         GLuint vbo;
//     };

//     std::unordered_map<PrimitiveType, float> m_vertexCountMap;

//     std::unordered_map<PrimitiveType, shapeBuffers> m_shapeBuffersMap;
//     std::vector<RenderShapeData> m_shapes;
//     // void generateShapes(const std::vector<RenderShapeData>& shapes);

//     void initializeShapeBuffers(const PrimitiveType &shapeType, const std::vector<float> &vertexData, const float vertexCount);

//     glm::mat3 rotationMatrix(const glm::vec3& axis, float angle);
//     void rotateCamera(float deltaX, float deltaY);

//     // FBO lab
//     void makeFBO();
//     // Task 30: Update the paintTexture function signature
//     void paintTexture(GLuint texture, bool toggle, bool kernel);

//     GLuint m_defaultFBO;
//     int m_fbo_width;
//     int m_fbo_height;
//     int m_screen_width;
//     int m_screen_height;
//     GLuint m_fullscreen_vbo;
//     GLuint m_fullscreen_vao;

//     GLuint m_texture_shader;
//     GLuint m_fbo;
//     GLuint m_fbo_texture;
//     GLuint m_fbo_renderbuffer;

// };


#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include "./utils/camera.h"
#include "./utils/shadermanager.h"
#include "shapes/building.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

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

    // Shader Manager
    ShaderManager& m_shaderManager = ShaderManager::getInstance();

    // VBO/VAO
    GLuint m_vbo;
    GLuint m_vao;

    glm::mat4 getProjectionMatrix(float near, float far);

    // transforamtion matrices
    glm::mat4 m_model = glm::mat4(1);
    glm::mat4 m_view  = glm::mat4(1);
    glm::mat4 m_proj  = glm::mat4(1);
    glm::mat4 mvp_matrix;

    Building* m_building;
    std::vector<RenderShapeData> m_shapes;
    void rotateCamera(float deltaX, float deltaY);

};
