#include <SFML/Graphics.hpp>
#include <vector>

class Map : public sf::Drawable, public sf::Transformable
{
    public:
        bool load(int* tiles, unsigned int width, unsigned int height, unsigned int tile_size);
        bool changeTileTexture(int tileID, int nTexture);

        bool isTilesetLoaded(){return tilesetLoaded;}

        int getTileFromPosition(sf::Vector2f position);

        Map(const std::filesystem::path& tileset);

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            states.transform *= getTransform();
            states.texture = &m_tileset;

            target.draw(m_verticies, states);
        }

        sf::VertexArray m_verticies;
        sf::Texture m_tileset;
        bool tilesetLoaded {false};

        int* m_tiles;

        unsigned int m_width;
        unsigned int m_height;
        unsigned int m_tileSize;
};