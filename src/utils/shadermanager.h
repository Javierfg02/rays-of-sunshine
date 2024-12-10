// shadermanager.h
#pragma once
#include <GL/glew.h>
#include <unordered_map>
#include <string>
#include "shaderloader.h"

class ShaderManager {
public:
    enum class ShaderType {
        BUILDING,
        HORIZONTAL_BLUR,
        VERTICAL_BLUR,
        DEPTH_OF_FIELD,
        CREPUSCULAR_RAYS,
        SKYBOX
    };

    // custom hash function for ShaderType - required for unordered map to work with enums
    struct ShaderTypeHash {
        template <typename T>
        std::size_t operator()(T t) const {
            return static_cast<std::size_t>(t);
        }
    };

    // For Singleton pattern - have only one instance of the class
    static ShaderManager& getInstance() {
        static ShaderManager instance; // static so only one version of the variable ever
        return instance;
    }

    // shaders
    GLuint dof_shader;
    GLuint building_shader;
    GLuint hblur_shader;
    GLuint vblur_shader;

    void initializeShaders() {
        // Buildings shader
        building_shader = createShaderProgram(ShaderType::BUILDING,
                            ":/resources/shaders/building.vert",
                            ":/resources/shaders/building.frag");

        // Horizontal blur shader
        hblur_shader = createShaderProgram(ShaderType::HORIZONTAL_BLUR,
                            ":/resources/shaders/hblur.vert",
                            ":/resources/shaders/hblur.frag");

        // Vertical blur shader
        vblur_shader = createShaderProgram(ShaderType::VERTICAL_BLUR,
                            ":/resources/shaders/vblur.vert",
                            ":/resources/shaders/vblur.frag");

        // Depth of field shader
        dof_shader = createShaderProgram(ShaderType::DEPTH_OF_FIELD,
                            ":/resources/shaders/dof.vert",
                            ":/resources/shaders/dof.frag");

        // // Crepuscular rays shader
        // createShaderProgram(ShaderType::CREPUSCULAR_RAYS,
        //                     ":/resources/shaders/rays.vert",
        //                     ":/resources/shaders/rays.frag");

        // // Skybox shader
        // createShaderProgram(ShaderType::SKYBOX,
        //                     ":/resources/shaders/skybox.vert",
        //                     ":/resources/shaders/skybox.frag");
    }

    /**
     * @brief getShader
     * @param type enum with values: BUILDING, DEPTH_OF_FIELD, CREPUSCULAR_RAYS & SKYBOX
     * @return The shader program's OpenGL ID
     */
    GLuint getShader(ShaderType type) {
        return m_shaderPrograms[type];
    }

    void useShader(ShaderType type) {
        glUseProgram(m_shaderPrograms[type]);
    }

private:
    ShaderManager() {} // for Singleton pattern

    // using custom hash function for the enum class
    std::unordered_map<ShaderType, GLuint, ShaderTypeHash> m_shaderPrograms;

    GLuint createShaderProgram(ShaderType type, const char * vertPath, const char * fragPath) {
        return m_shaderPrograms[type] = ShaderLoader::createShaderProgram(vertPath, fragPath);
    }
};
