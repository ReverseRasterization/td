#include "map.h"

bool Map::load(/*const std::filesystem::path& tileset, const int* tiles, */unsigned int width, unsigned int height, unsigned int tile_size)
{
    // Load the texture
    // if(!m_tileset.loadFromFile(tileset))
    // {
    //     return false;
    // }

    // Prepare vertex array
    m_verticies.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_verticies.resize(width*height*6);

    // Populate vertex array
    for (unsigned int i = 0; i < width; i++)
    {
        for (unsigned int j = 0; j < height; j++)
        {
            sf::Vertex* triangles = &m_verticies[(i+j * width) * 6];

            // Verticies are setup in a counter-clockwise order
            triangles[0].position = sf::Vector2f(i*tile_size, j * tile_size); // Top Left
            triangles[1].position = sf::Vector2f(i*tile_size, (j+1)*tile_size); // Bottom Left
            triangles[2].position = sf::Vector2f((i+1)*tile_size, (j+1)*tile_size); // Bottom Right
            
            triangles[3].position = sf::Vector2f(i*tile_size, j * tile_size); // Top Left
            triangles[4].position = sf::Vector2f((i+1)*tile_size, (j+1)*tile_size); // Bottom Right
            triangles[5].position = sf::Vector2f((i+1)*tile_size, j*tile_size); // Top Right

            triangles[0].color = sf::Color::Red;
            triangles[1].color = sf::Color::White;
            triangles[2].color = sf::Color::Blue;

            triangles[3].color = sf::Color::Red;
            triangles[4].color = sf::Color::White;
            triangles[5].color = sf::Color::Blue;
        }
    }

    return true;
}