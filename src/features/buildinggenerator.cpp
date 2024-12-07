#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "buildinggenerator.h"
#include "./shapes/building.h"
#include "./settings.h"

BuildingGenerator::BuildingGenerator() {
    generateGrid();
}

std::vector<float> BuildingGenerator::generateBuildings() {
    int numBuildings = settings.numBuildings;

    for (int i = 0; i < numBuildings; i++) {
        Building* building = new Building();
        building->updateParams(); // generates pseudorandomized building
        std::vector<float> buildingData = building->generateShape();

        // position the buildings
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
            m_buildingData.push_back(pos.x);
            m_buildingData.push_back(pos.y);
            m_buildingData.push_back(pos.z);
            m_buildingData.push_back(buildingData[j+3]);
            m_buildingData.push_back(buildingData[j+4]);
            m_buildingData.push_back(buildingData[j+5]);
        }

        delete building;
    }

    return m_buildingData;
}

void BuildingGenerator::generateGrid() {}
