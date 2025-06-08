#include <SFML/Graphics.hpp>

class Map : public sf::Drawable, public sf::Transformable
{
    public:
        bool load(/*const std::filesystem::path& tileset, const int* tiles, */unsigned int width, unsigned int height, unsigned int tile_size);

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            states.transform *= getTransform();
            states.texture = &m_tileset;

            target.draw(m_verticies, states);

        }

        sf::VertexArray m_verticies;
        sf::Texture m_tileset;
};