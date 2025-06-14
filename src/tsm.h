#pragma once
#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>

class TextureSelectionManager : public sf::Drawable, public sf::Transformable
{
    public:
        struct TextureGroup
        {
            std::string s_name;
            std::vector<int> s_textureIDs;

            TextureGroup(std::string name, std::vector<int> textureIDs): s_name(name), s_textureIDs(textureIDs){};
        };

        void registerTextureGroup(TextureGroup new_texture_group, bool is_overlay);
        void setOverlayMode(bool overlayMode);

        void scrollTextureGroup(int factor);
        void scrollTexture(int factor);

        int getCurrentTexture();


        TextureSelectionManager(sf::Texture& tileset);

    private:

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            states.transform *= getTransform();
            states.texture = &p_tileset;

            target.draw(p_verticies, states);
        }

        void updateSelectionPreview(int nTex);


        sf::VertexArray p_verticies;
        sf::Texture p_tileset;

        std::vector<TextureGroup> textureGroups;
        std::vector<TextureGroup> overlayTextureGroups;

        int currGroup{0};
        int currOverlayGroup{0};
        int currTextureIndex{0};
        int currOverlayTextureIndex{0};

        bool tsm_overlayMode{false};
}; 