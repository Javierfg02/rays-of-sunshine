cmake_minimum_required(VERSION 3.16)

# Sets project name
project(projects_realtime LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Sets C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifies required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Xml)

add_definitions(-DPROJECT_PATH="${CMAKE_SOURCE_DIR}")

# Allows you to include files from within those directories, without prefixing their filepaths
include_directories(src)

# Specifies .cpp and .h files to be passed to the compiler
add_executable(${PROJECT_NAME}
    src/main.cpp

    src/realtime.cpp
    src/mainwindow.cpp
    src/settings.cpp
    src/utils/scenefilereader.cpp
    src/utils/sceneparser.cpp
    src/shapes/Cylinder.cpp
    src/shapes/Cube.cpp
    src/shapes/Cone.cpp
    src/shapes/Sphere.cpp
    src/shapes/Triangle.cpp
    src/shapes/Tet.cpp

    src/mainwindow.h
    src/realtime.h
    src/settings.h
    src/utils/scenedata.h
    src/utils/scenefilereader.h
    src/utils/sceneparser.h
    src/utils/shaderloader.h
    src/utils/aspectratiowidget.hpp
    src/shapes/Cylinder.h
    src/shapes/Cube.h
    src/shapes/Sphere.h
    src/shapes/Triangle.h
    src/shapes/Tet.h
    src/utils/camera.h src/utils/camera.cpp
    src/shapes/building.h src/shapes/building.cpp
)

# GLM: this creates its library and allows you to `#include "glm/..."`
add_subdirectory(glm)

# GLEW: this creates its library and allows you to `#include "GL/glew.h"`
add_library(StaticGLEW STATIC glew/src/glew.c
    src/utils/debug.h

    src/features/buildinggenerator.h src/features/buildinggenerator.cpp
    src/utils/shadermanager.h
    resources/shaders/building.vert
    resources/shaders/building.frag
    resources/shaders/default.frag
    resources/shaders/default.vert
    resources/shaders/dof.frag
    resources/shaders/dof.vert
    src/utils/camera.h src/utils/camera.cpp
    src/features/bezier.h src/features/bezier.cpp
    # src/utils/graphicsengine.h src/utils/graphicsengine.cpp
    # src/utils/debug.h
    # src/shapes/cone.h
)
include_directories(${PROJECT_NAME} PRIVATE glew/include)

target_sources(StaticGLEW
  PRIVATE
    # src/utils/camera.h src/utils/camera.cpp
)

# Specifies libraries to be linked (Qt components, glew, etc)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::OpenGL
    Qt::OpenGLWidgets
    Qt::Xml
    StaticGLEW
)

# Specifies other files
qt6_add_resources(${PROJECT_NAME} "Resources"
    PREFIX
        "/"
    FILES
        resources/shaders/default.frag
        resources/shaders/default.vert
        resources/shaders/building.frag
        resources/shaders/building.vert
        resources/shaders/dof.frag
        resources/shaders/dof.vert
)

# GLEW: this provides support for Windows (including 64-bit)
if (WIN32)
  add_compile_definitions(GLEW_STATIC)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    opengl32
    glu32
  )
endif()

# Set this flag to silence warnings on Windows
if (MSVC OR MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "-Wno-volatile")
endif()
# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()
