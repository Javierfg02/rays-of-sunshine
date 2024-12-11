#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "buildinggenerator.h"
#include "./shapes/building.h"
#include "./settings.h"

BuildingGenerator::BuildingGenerator(float citySize) {
    this->citySize = citySize;
    this->generateGrid();
}

std::vector<float> BuildingGenerator::initializeBuildings() {
    std::vector<float> vbo_data;
    int numBuildings = 0;

    // Use reference to modify actual grid cells
    for(int i = 0; i < m_grid.size(); i++) {
        GridCell& cell = m_grid[i];
        // if the cell is not a road cell and there is a building defined
        if(!cell.isRoad && cell.building && !cell.isInit) {
            numBuildings++;
            cell.isInit = true;
            std::vector<float> buildingData = cell.building->generateShape();

            // Calculate world position based on grid position
            float xPos = cell.col * (settings.buildingMaxWidth);
            float zPos = cell.row * (settings.buildingMaxWidth);
            glm::vec3 position(xPos, 0.0f, zPos);

            // apply position to all vertices of this building
            for(int i = 0; i < buildingData.size(); i += 6) {
                glm::vec4 pos(buildingData[i], buildingData[i+1], buildingData[i+2], 1.0f);
                glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
                pos = transform * pos;

                // push vertex data
                vbo_data.push_back(pos.x);
                vbo_data.push_back(pos.y);
                vbo_data.push_back(pos.z);

                // push color data
                vbo_data.push_back(buildingData[i+3]);
                vbo_data.push_back(buildingData[i+4]);
                vbo_data.push_back(buildingData[i+5]);
            }
        }
    }
    std::cout << "num buildings generated: " << numBuildings << std::endl;
    return vbo_data;
}

void BuildingGenerator::generateGrid() {
    int gridSize = this->citySize / settings.buildingMaxWidth;

    m_grid.clear();

    // determine which rows and columns are roads
    int numRoads = 0;
    std::vector<bool> roadRows(gridSize, false);
    std::vector<bool> roadCols(gridSize, false);

    // make middle row always a road -- camera starts here
    int middleRow = gridSize / 2;
    roadRows[middleRow] = true;

    // randomly select road rows
    for (int i = 0; i < gridSize; i++) {
        if (rand() % 7 == 0) { // 14% chance for each row to be a road
            roadRows[i] = true;
            numRoads++;
        }
    }

    // randomly select road columns
    for (int j = 0; j < gridSize; j++) {
        if (rand() % 7 == 0) { // 14% chance for each column to be a road
            roadCols[j] = true;
            numRoads++;
        }
    }

    for (int row = 0; row < gridSize; row++) {
        for (int col = 0; col < gridSize; col++) {
            GridCell cell;
            cell.row = row;
            cell.col = col;
            cell.isRoad = roadRows[row] || roadCols[col];
            cell.width = settings.buildingMaxWidth;  // standard cell size = max building size
            cell.height = settings.buildingMaxWidth;

            if (!cell.isRoad) {
                // create a building for non-road cell
                cell.building = new Building();
                cell.building->updateParams(); // randomize building params
            } else {
                cell.building = nullptr;
            }

            m_grid.push_back(cell);
        }
    }
    std::cout << "grid rows " << m_grid.size() / 2 << std::endl;
    std::cout << "num roads generated: " << numRoads << std::endl;
}

glm::vec3 BuildingGenerator::getRandomRoadPosition() {
    int gridSize = this->citySize / settings.buildingMaxWidth;

    // Always return a position on the middle road, at the edge
    float xPos = 0; // Start of the road
    float zPos = (gridSize / 2) * settings.buildingMaxWidth;
    return glm::vec3(xPos, 0.0f, zPos);
}
