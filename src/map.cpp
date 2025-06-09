#include "map.h"
#include <iostream>
#include <cmath>

Map::Map(const std::filesystem::path& tileset)
{
    if(!m_tileset.loadFromFile(tileset))
    {
        std::cerr << "Failed to load tileset!\n";
    }else {
        tilesetLoaded = true;
    }
}

bool Map::load(int* tiles, unsigned int width, unsigned int height, unsigned int tile_size)
{
    // Register variables
    m_width = width;
    m_height = height;
    m_tileSize = tile_size;
    m_tiles = tiles;
    
    // Prepare vertex array
    m_verticies.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_verticies.resize(width*height*6);

    // Populate vertex array
    for (unsigned int i = 0; i < width; i++)
    {
        for (unsigned int j = 0; j < height; j++)
        {
            sf::Vertex* triangles = &m_verticies[(i+j * width) * 6];

            // Get the texture
            const int tileNum = tiles[i + j * width]; // segmentation fault here

            float row = std::floor(tileNum/18); // 18 columns
            float column = std::floor(tileNum%18);

            sf::Vector2f textureTL {column * 17.f, row * 17.f}; // you have 17 bc its tile size + spacing (16 + 1)
            sf::Vector2f textureBL {textureTL.x, textureTL.y + 16};
            sf::Vector2f textureTR {textureTL.x + 16, textureTL.y};
            sf::Vector2f textureBR {textureTL.x + 16, textureTL.y + 16};
            
            // Verticies are setup in a counter-clockwise order
            triangles[0].position = sf::Vector2f(i*tile_size, j * tile_size); // Top Left
            triangles[1].position = sf::Vector2f(i*tile_size, (j+1)*tile_size); // Bottom Left
            triangles[2].position = sf::Vector2f((i+1)*tile_size, (j+1)*tile_size); // Bottom Right
            
            triangles[3].position = sf::Vector2f(i*tile_size, j * tile_size); // Top Left
            triangles[4].position = sf::Vector2f((i+1)*tile_size, (j+1)*tile_size); // Bottom Right
            triangles[5].position = sf::Vector2f((i+1)*tile_size, j*tile_size); // Top Right

            triangles[0].texCoords = textureTL;
            triangles[1].texCoords = textureBL;
            triangles[2].texCoords = textureBR;

            triangles[3].texCoords = textureTL;
            triangles[4].texCoords = textureBR;
            triangles[5].texCoords = textureTR;
        }
    }

    return true;
}

int Map::getTileFromPosition(sf::Vector2f position)
{
    int column = static_cast<int>(position.x / m_tileSize);
    int row = static_cast<int>(position.y / m_tileSize);

    return row * m_width + column;
}

bool Map::changeTileTexture(int tileID, int nTexture)
{
    m_tiles[tileID] = nTexture;

    return load(m_tiles, m_width, m_height, m_tileSize);
}