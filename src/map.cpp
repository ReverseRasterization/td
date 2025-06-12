#include "map.h"
#include <iostream>
#include <cmath>
#include <algorithm>

void Map::load(std::vector<Tile> tiles, unsigned int width, unsigned int height, unsigned int tile_size)
{
    // Register variables
    m_width = width;
    m_height = height;
    m_tileSize = tile_size;
    m_tiles = tiles;
    
    // Prepare vertex array
    g_verticies.setPrimitiveType(sf::PrimitiveType::Triangles);
    g_verticies.resize(width*height*6);

    o_verticies.setPrimitiveType(sf::PrimitiveType::Triangles);
    o_verticies.resize((width * height * 6));

    // Populate vertex array
    for (unsigned int row = 0; row < width; row++)
    {
        for (unsigned int column = 0; column < height; column++)
        {
            // Drawing the ground

            sf::Vertex* triangles = &g_verticies[(row+column * width) * 6];

            // Get the texture
            int texID = tiles[row + column * width].groundTexID;

            float texRow = std::floor(texID/18); // 18 columns
            float texColumn = std::floor(texID%18);

            sf::Vector2f textureTL {texColumn * 17.f, texRow * 17.f}; // you have 17 bc its tile size + spacing (16 + 1)
            sf::Vector2f textureBL {textureTL.x, textureTL.y + 16};
            sf::Vector2f textureTR {textureTL.x + 16, textureTL.y};
            sf::Vector2f textureBR {textureTL.x + 16, textureTL.y + 16};
            
            // Verticies are setup in a counter-clockwise order
            triangles[0].position = sf::Vector2f(row*tile_size, column * tile_size); // Top Left
            triangles[1].position = sf::Vector2f(row*tile_size, (column+1)*tile_size); // Bottom Left
            triangles[2].position = sf::Vector2f((row+1)*tile_size, (column+1)*tile_size); // Bottom Right
            
            triangles[3].position = sf::Vector2f(row*tile_size, column * tile_size); // Top Left
            triangles[4].position = sf::Vector2f((row+1)*tile_size, (column+1)*tile_size); // Bottom Right
            triangles[5].position = sf::Vector2f((row+1)*tile_size, column*tile_size); // Top Right

            triangles[0].texCoords = textureTL;
            triangles[1].texCoords = textureBL;
            triangles[2].texCoords = textureBR;

            triangles[3].texCoords = textureTL;
            triangles[4].texCoords = textureBR;
            triangles[5].texCoords = textureTR;

            // Drawing the overlay

            if (tiles[row + column * width].overlay)
            {
                triangles = &o_verticies[(row+column * width) * 6];

                // Get the texture
                int texID = tiles[row + column * width].overlayTexID;

                float texRow = std::floor(texID/18); // 18 columns
                float texColumn = std::floor(texID%18);

                sf::Vector2f textureTL {texColumn * 17.f, texRow * 17.f}; // you have 17 bc its tile size + spacing (16 + 1)
                sf::Vector2f textureBL {textureTL.x, textureTL.y + 16};
                sf::Vector2f textureTR {textureTL.x + 16, textureTL.y};
                sf::Vector2f textureBR {textureTL.x + 16, textureTL.y + 16};

                // Verticies are setup in a counter-clockwise order
                triangles[0].position = sf::Vector2f(row*tile_size, column * tile_size); // Top Left
                triangles[1].position = sf::Vector2f(row*tile_size, (column+1)*tile_size); // Bottom Left
                triangles[2].position = sf::Vector2f((row+1)*tile_size, (column+1)*tile_size); // Bottom Right
                
                triangles[3].position = sf::Vector2f(row*tile_size, column * tile_size); // Top Left
                triangles[4].position = sf::Vector2f((row+1)*tile_size, (column+1)*tile_size); // Bottom Right
                triangles[5].position = sf::Vector2f((row+1)*tile_size, column*tile_size); // Top Right

                triangles[0].texCoords = textureTL;
                triangles[1].texCoords = textureBL;
                triangles[2].texCoords = textureBR;

                triangles[3].texCoords = textureTL;
                triangles[4].texCoords = textureBR;
                triangles[5].texCoords = textureTR;
            }
        }
    }
}

void Map::changeTileTexture(int tileID, bool overlay, int nTexture)
{
    if (overlay)
    {
        nTexture > 0 ? m_tiles[tileID].overlay = true : m_tiles[tileID].overlay = false;

        m_tiles[tileID].overlayTexID = nTexture;
    }else {
        m_tiles[tileID].groundTexID = nTexture;
    }

    g_verticies.clear();
    o_verticies.clear();

    load(m_tiles, m_width, m_height, m_tileSize);
}

int Map::getTileFromPosition(sf::Vector2f position)
{
    int column = static_cast<int>(position.x / m_tileSize);
    int row = static_cast<int>(position.y / m_tileSize);

    return row * m_width + column;
}

