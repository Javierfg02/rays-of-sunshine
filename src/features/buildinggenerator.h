#include <GL/glew.h>
#include <glm/glm.hpp>
#include "utils/sceneparser.h"
#include "utils/scenedata.h"


#ifndef BUILDINGGENERATOR_H
#define BUILDINGGENERATOR_H

class BuildingGenerator {

public:
    struct BuildingParams {
        float minHeight = 10.0f;
        float maxHeight = 30.0f;
        float minWidth = 5.0f;
        float maxWidth = 15.0f;
        float minDepth = 8.0f;
        float maxDepth = 20.0f;
        float floorHeight = 3.0f;
        float windowWidth = 1.2f;
        float windowHeight = 1.8f;
        float windowSpacing = 1.0f;
        float streetWidth = 20.0f;  // Width of the road
    };

    struct BuildingVertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    struct Building {
        std::vector<BuildingVertex> vertices;
        std::vector<GLuint> indices;
        glm::vec3 position;
        float height;
        float width;
        float depth;
    };

    BuildingGenerator();
    ~BuildingGenerator();

    void init(GLuint shader);
    void cleanup();
    void renderBuildings();
    void initializeBuildings();
    GLuint getShader();

private:
    GLuint m_shader;
    GLbuffers buildingBuffers;

};

#endif // BUILDINGGENERATOR_H
