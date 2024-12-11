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
            float rndPosOffset = (rand() % 4) - 4/2.f; // add some randomness to the x and y positions of buildings
            for(int i = 0; i < buildingData.size(); i += 9) {
                glm::vec4 pos(buildingData[i] + rndPosOffset, buildingData[i+1], buildingData[i+2] + rndPosOffset, 1.0f);
                glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
                pos = transform * pos;

                // push vertex data
                vbo_data.push_back(pos.x);
                vbo_data.push_back(pos.y);
                vbo_data.push_back(pos.z);

                // push normal data (offset by 3)
                vbo_data.push_back(buildingData[i+3]);
                vbo_data.push_back(buildingData[i+4]);
                vbo_data.push_back(buildingData[i+5]);

                // push color data (offset by 6)
                vbo_data.push_back(buildingData[i+6]);
                vbo_data.push_back(buildingData[i+7]);
                vbo_data.push_back(buildingData[i+8]);
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
    if (rand() % 7 == 0) {// 14% chance for each row to be a road
            roadRows[i] = true;
            numRoads++;
        }
    }

    // randomly select road columns
    for (int j = 0; j < gridSize; j++) {
    if (rand() % 7 == 0) { // 14% chance for each column to be a road
            roadCols[j] = true;
            numRoads++;
            std::cout << "num roads" << numRoads << std::endl;
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

    float xPos = 0; // start of the road
    float zPos = (gridSize / 2) * settings.buildingMaxWidth;
    return glm::vec3(xPos, 0.0f, zPos);
}

std::vector<float> BuildingGenerator::getRoadData() {
    m_roadVertexData.clear();
    this->generateRoad();
    std::cout << "Road vertex data size: " << m_roadVertexData.size() << std::endl;
    return m_roadVertexData;
}

// std::vector<float> BuildingGenerator::generateRoad() {
//     std::vector<float> roadVertexData;

//     // float roadWidth = 4.0f;
//     // float roadHeight = 1.f;
//     // float totalLength = settings.buildingMaxWidth * 100;

//     // // Road surface
//     // glm::vec3 roadColor(1.f, 1.f, 1.f);
//     // glm::vec3 upNormal(0.0f, 1.0f, 0.0f);
//     float roadWidth = 8.0f;  // Wider road
//     float roadHeight = 5.0f;  // Lower height to be just above ground
//     // float totalLength = settings.buildingMaxWidth * 100;

//     // Make road much brighter for testing
//     // glm::vec3 roadColor(0.6f, 0.6f, 0.6f);  // Light gray
//     glm::vec3 lineColor(1.0f, 1.0f, 0.0f);  // Bright yellow
//     glm::vec3 curbColor(0.5f, 0.5f, 0.5f);  // Light gray curbs

//     // Get grid dimensions
//     int gridSize = this->citySize / settings.buildingMaxWidth;
//     float zOffset = (gridSize / 2) * settings.buildingMaxWidth;  // Middle row position
//     float totalLength = settings.buildingMaxWidth * gridSize;  // Match city size

//     // Center road at middle row
//     glm::vec3 roadColor(1.0f, 1.0f, 1.0f);
//     glm::vec3 upNormal(0.0f, 1.0f, 0.0f);



//     // Main road surface - positioned at middle row
//     addQuad(
//         glm::vec3(-roadWidth, roadHeight, -zOffset/2),
//         glm::vec3(roadWidth, roadHeight, -zOffset/2),
//         glm::vec3(-roadWidth, roadHeight, totalLength - zOffset/2),
//         glm::vec3(roadWidth, roadHeight, totalLength - zOffset/2),
//         upNormal,
//         roadColor);

//     // Main road surface
//     addQuad(
//         glm::vec3(-roadWidth, roadHeight, 0.0f),
//         glm::vec3(roadWidth, roadHeight, 0.0f),
//         glm::vec3(-roadWidth, roadHeight, totalLength),
//         glm::vec3(roadWidth, roadHeight, totalLength),
//         upNormal,
//         roadColor);

//     // Center line
//     float lineWidth = 0.5f;
//     float lineHeight = roadHeight + 0.01f;
//     // glm::vec3 lineColor(0.9f, 0.9f, 0.7f);

//     addQuad(
//         glm::vec3(-lineWidth/2, lineHeight, 0.0f),
//         glm::vec3(lineWidth/2, lineHeight, 0.0f),
//         glm::vec3(-lineWidth/2, lineHeight, totalLength),
//         glm::vec3(lineWidth/2, lineHeight, totalLength),
//         upNormal,
//         lineColor);

//     // side curbs
//     float curbHeight = 1.1f;
//     // glm::vec3 curbColor(0.3f, 0.3f, 0.3f);

//     // Left curb
//     addQuad(
//         glm::vec3(-roadWidth-0.2f, 0.0f, 0.0f),
//         glm::vec3(-roadWidth, curbHeight, 0.0f),
//         glm::vec3(-roadWidth-0.2f, 0.0f, totalLength),
//         glm::vec3(-roadWidth, curbHeight, totalLength),
//         glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)),
//         curbColor);

//     // Right curb
//     addQuad(
//         glm::vec3(roadWidth, curbHeight, 0.0f),
//         glm::vec3(roadWidth+0.2f, 0.0f, 0.0f),
//         glm::vec3(roadWidth, curbHeight, totalLength),
//         glm::vec3(roadWidth+0.2f, 0.0f, totalLength),
//         glm::normalize(glm::vec3(-1.0f, 1.0f, 0.0f)),
//         curbColor);

//     return roadVertexData;
// }


std::vector<float> BuildingGenerator::generateRoad() {
    std::vector<float> roadVertexData;

    // float roadWidth = 4.0f;
    // float roadHeight = 1.f;
    // float totalLength = settings.buildingMaxWidth * 100;

    // // Road surface
    // glm::vec3 roadColor(1.f, 1.f, 1.f);
    // glm::vec3 upNormal(0.0f, 1.0f, 0.0f);
    float roadWidth = 8.0f;  // Wider road
    float roadHeight = 5.0f;  // Lower height to be just above ground
    // float totalLength = settings.buildingMaxWidth * 100;

    // Make road much brighter for testing
    // glm::vec3 roadColor(0.6f, 0.6f, 0.6f);  // Light gray
    glm::vec3 lineColor(1.0f, 1.0f, 0.0f);  // Bright yellow
    glm::vec3 curbColor(0.5f, 0.5f, 0.5f);  // Light gray curbs

    // Get grid dimensions
    int gridSize = this->citySize / settings.buildingMaxWidth;
    float zOffset = (gridSize / 2) * settings.buildingMaxWidth;  // Middle row position
    float totalLength = settings.buildingMaxWidth * gridSize;  // Match city size

    // Center road at middle row
    glm::vec3 roadColor(1.0f, 1.0f, 1.0f);
    glm::vec3 upNormal(0.0f, 1.0f, 0.0f);



    // Main road surface - positioned at middle row
    addQuad(
        glm::vec3(-roadWidth, roadHeight, -zOffset/2),
        glm::vec3(roadWidth, roadHeight, -zOffset/2),
        glm::vec3(-roadWidth, roadHeight, totalLength - zOffset/2),
        glm::vec3(roadWidth, roadHeight, totalLength - zOffset/2),
        upNormal,
        roadColor);

    // Main road surface
    addQuad(
        glm::vec3(-roadWidth, roadHeight, 0.0f),
        glm::vec3(roadWidth, roadHeight, 0.0f),
        glm::vec3(-roadWidth, roadHeight, totalLength),
        glm::vec3(roadWidth, roadHeight, totalLength),
        upNormal,
        roadColor);

    // Center line
    float lineWidth = 0.5f;
    float lineHeight = roadHeight + 0.01f;
    // glm::vec3 lineColor(0.9f, 0.9f, 0.7f);

    addQuad(
        glm::vec3(-lineWidth/2, lineHeight, 0.0f),
        glm::vec3(lineWidth/2, lineHeight, 0.0f),
        glm::vec3(-lineWidth/2, lineHeight, totalLength),
        glm::vec3(lineWidth/2, lineHeight, totalLength),
        upNormal,
        lineColor);

    // side curbs
    float curbHeight = 1.1f;
    // glm::vec3 curbColor(0.3f, 0.3f, 0.3f);

    // Left curb
    addQuad(
        glm::vec3(-roadWidth-0.2f, 0.0f, 0.0f),
        glm::vec3(-roadWidth, curbHeight, 0.0f),
        glm::vec3(-roadWidth-0.2f, 0.0f, totalLength),
        glm::vec3(-roadWidth, curbHeight, totalLength),
        glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)),
        curbColor);

    // Right curb
    addQuad(
        glm::vec3(roadWidth, curbHeight, 0.0f),
        glm::vec3(roadWidth+0.2f, 0.0f, 0.0f),
        glm::vec3(roadWidth, curbHeight, totalLength),
        glm::vec3(roadWidth+0.2f, 0.0f, totalLength),
        glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)),
        curbColor);

    return roadVertexData;
}

void BuildingGenerator::addQuad(glm::vec3 bl, glm::vec3 br, glm::vec3 tl, glm::vec3 tr,
                                glm::vec3 normal, glm::vec3 color) {

    std::cout << "Road quad vertices:" << std::endl;
    std::cout << "Bottom Left: " << bl.x << ", " << bl.y << ", " << bl.z << std::endl;
    std::cout << "Bottom Right: " << br.x << ", " << br.y << ", " << br.z << std::endl;
    std::cout << "Top Left: " << tl.x << ", " << tl.y << ", " << tl.z << std::endl;
    std::cout << "Top Right: " << tr.x << ", " << tr.y << ", " << tr.z << std::endl;
    std::cout << "Normal: " << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
    std::cout << "Color: " << color.r << ", " << color.g << ", " << color.b << std::endl;

    // First triangle (bl, br, tr)
    // bottom left vertex
    m_roadVertexData.push_back(bl.x);
    m_roadVertexData.push_back(bl.y);
    m_roadVertexData.push_back(bl.z);
    m_roadVertexData.push_back(normal.x);
    m_roadVertexData.push_back(normal.y);
    m_roadVertexData.push_back(normal.z);
    m_roadVertexData.push_back(color.r);
    m_roadVertexData.push_back(color.g);
    m_roadVertexData.push_back(color.b);

    // bottom right vertex
    m_roadVertexData.push_back(br.x);
    m_roadVertexData.push_back(br.y);
    m_roadVertexData.push_back(br.z);
    m_roadVertexData.push_back(normal.x);
    m_roadVertexData.push_back(normal.y);
    m_roadVertexData.push_back(normal.z);
    m_roadVertexData.push_back(color.r);
    m_roadVertexData.push_back(color.g);
    m_roadVertexData.push_back(color.b);

    // top right vertex
    m_roadVertexData.push_back(tr.x);
    m_roadVertexData.push_back(tr.y);
    m_roadVertexData.push_back(tr.z);
    m_roadVertexData.push_back(normal.x);
    m_roadVertexData.push_back(normal.y);
    m_roadVertexData.push_back(normal.z);
    m_roadVertexData.push_back(color.r);
    m_roadVertexData.push_back(color.g);
    m_roadVertexData.push_back(color.b);

    // second triangle (bl, tr, tl)
    // bottom left vertex
    m_roadVertexData.push_back(bl.x);
    m_roadVertexData.push_back(bl.y);
    m_roadVertexData.push_back(bl.z);
    m_roadVertexData.push_back(normal.x);
    m_roadVertexData.push_back(normal.y);
    m_roadVertexData.push_back(normal.z);
    m_roadVertexData.push_back(color.r);
    m_roadVertexData.push_back(color.g);
    m_roadVertexData.push_back(color.b);

    // top right vertex
    m_roadVertexData.push_back(tr.x);
    m_roadVertexData.push_back(tr.y);
    m_roadVertexData.push_back(tr.z);
    m_roadVertexData.push_back(normal.x);
    m_roadVertexData.push_back(normal.y);
    m_roadVertexData.push_back(normal.z);
    m_roadVertexData.push_back(color.r);
    m_roadVertexData.push_back(color.g);
    m_roadVertexData.push_back(color.b);

    // top left vertex
    m_roadVertexData.push_back(tl.x);
    m_roadVertexData.push_back(tl.y);
    m_roadVertexData.push_back(tl.z);
    m_roadVertexData.push_back(normal.x);
    m_roadVertexData.push_back(normal.y);
    m_roadVertexData.push_back(normal.z);
    m_roadVertexData.push_back(color.r);
    m_roadVertexData.push_back(color.g);
    m_roadVertexData.push_back(color.b);
}


// void BuildingGenerator::addQuad(glm::vec3 bl, glm::vec3 br, glm::vec3 tl, glm::vec3 tr,
//     glm::vec3 normal, glm::vec3 color) {

//     std::cout << "Road quad vertices:" << std::endl;
//     std::cout << "Bottom Left: " << bl.x << ", " << bl.y << ", " << bl.z << std::endl;
//     std::cout << "Bottom Right: " << br.x << ", " << br.y << ", " << br.z << std::endl;
//     std::cout << "Top Left: " << tl.x << ", " << tl.y << ", " << tl.z << std::endl;
//     std::cout << "Top Right: " << tr.x << ", " << tr.y << ", " << tr.z << std::endl;
//     std::cout << "Normal: " << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
//     std::cout << "Color: " << color.r << ", " << color.g << ", " << color.b << std::endl;

//     // First triangle (bl, br, tr)
//     // bottom left vertex
//     m_roadVertexData.push_back(bl.x);
//     m_roadVertexData.push_back(bl.y);
//     m_roadVertexData.push_back(bl.z);
//     m_roadVertexData.push_back(normal.x);
//     m_roadVertexData.push_back(normal.y);
//     m_roadVertexData.push_back(normal.z);
//     m_roadVertexData.push_back(color.r);
//     m_roadVertexData.push_back(color.g);
//     m_roadVertexData.push_back(color.b);

//     // bottom right vertex
//     m_roadVertexData.push_back(br.x);
//     m_roadVertexData.push_back(br.y);
//     m_roadVertexData.push_back(br.z);
//     m_roadVertexData.push_back(normal.x);
//     m_roadVertexData.push_back(normal.y);
//     m_roadVertexData.push_back(normal.z);
//     m_roadVertexData.push_back(color.r);
//     m_roadVertexData.push_back(color.g);
//     m_roadVertexData.push_back(color.b);

//     // top right vertex
//     m_roadVertexData.push_back(tr.x);
//     m_roadVertexData.push_back(tr.y);
//     m_roadVertexData.push_back(tr.z);
//     m_roadVertexData.push_back(normal.x);
//     m_roadVertexData.push_back(normal.y);
//     m_roadVertexData.push_back(normal.z);
//     m_roadVertexData.push_back(color.r);
//     m_roadVertexData.push_back(color.g);
//     m_roadVertexData.push_back(color.b);

//     // second triangle (bl, tr, tl)
//     // bottom left vertex
//     m_roadVertexData.push_back(bl.x);
//     m_roadVertexData.push_back(bl.y);
//     m_roadVertexData.push_back(bl.z);
//     m_roadVertexData.push_back(normal.x);
//     m_roadVertexData.push_back(normal.y);
//     m_roadVertexData.push_back(normal.z);
//     m_roadVertexData.push_back(color.r);
//     m_roadVertexData.push_back(color.g);
//     m_roadVertexData.push_back(color.b);

//     // top right vertex
//     m_roadVertexData.push_back(tr.x);
//     m_roadVertexData.push_back(tr.y);
//     m_roadVertexData.push_back(tr.z);
//     m_roadVertexData.push_back(normal.x);
//     m_roadVertexData.push_back(normal.y);
//     m_roadVertexData.push_back(normal.z);
//     m_roadVertexData.push_back(color.r);
//     m_roadVertexData.push_back(color.g);
//     m_roadVertexData.push_back(color.b);

//     // top left vertex
//     m_roadVertexData.push_back(tl.x);
//     m_roadVertexData.push_back(tl.y);
//     m_roadVertexData.push_back(tl.z);
//     m_roadVertexData.push_back(normal.x);
//     m_roadVertexData.push_back(normal.y);
//     m_roadVertexData.push_back(normal.z);
//     m_roadVertexData.push_back(color.r);
//     m_roadVertexData.push_back(color.g);
//     m_roadVertexData.push_back(color.b);
// }

glm::mat4 BuildingGenerator::getRandomRotatedTransform(float x, float z) {
    // random rotation between -5 and 5 degrees
    float randomAngle = (((float)rand() / RAND_MAX) * 10.0f - 5.0f) * (M_PI / 180.0f);

    // first translate to position
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));

    // then rotate around Y axis
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), randomAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    // combine transformations: translation * rotation
    return translation * rotation;
}
