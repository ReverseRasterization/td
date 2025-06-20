#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Map : public sf::Drawable, public sf::Transformable
{
    public:
        struct Tile
        {
            uint16_t groundTexID;
            uint16_t overlayTexID;
            bool overlay;

            Tile(uint16_t ground_texture, uint16_t overlay_texture, bool has_overlay):groundTexID(ground_texture), overlayTexID(overlay_texture), overlay(has_overlay){};
        };

        void load(std::vector<Tile> tiles, unsigned int width, unsigned int height, unsigned int tile_size);
        void changeTileTexture(int tileID, bool overlay, int nTexture);

        int getTileFromPosition(sf::Vector2f position);
        int getTileTexture(int tileID, bool overlay){return overlay ? m_tiles[tileID].overlayTexID : m_tiles[tileID].groundTexID;}

        unsigned int getWidth(){return m_width;}
        unsigned int getHeight(){return m_height;}

        Map(sf::Texture& tileset):m_tileset(tileset){};

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            states.transform *= getTransform();
            states.texture = &m_tileset;

            target.draw(g_verticies, states);
            target.draw(o_verticies, states);
        }

        sf::VertexArray g_verticies;
        sf::VertexArray o_verticies;
        sf::Texture m_tileset;

        std::vector<Tile> m_tiles;

        unsigned int m_width;
        unsigned int m_height;
        unsigned int m_tileSize;
};