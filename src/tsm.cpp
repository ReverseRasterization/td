#include "tsm.h"
#include <cmath>

TextureSelectionManager::TextureSelectionManager(sf::Texture& tileset, sf::Text& texture_group_text)
{
    texGroupText = &texture_group_text;
    p_tileset = tileset;


    previewBackground.setSize({100.f, 100.f});
    previewBackground.setPosition({700.f, 0.f});

    p_verticies.setPrimitiveType(sf::PrimitiveType::Triangles);
    p_verticies.resize(6);

    p_verticies[0].position = {725.f, 25.f};
    p_verticies[1].position = {725.f, 75.f};
    p_verticies[2].position = {775.f, 75.f};
    p_verticies[3].position = {725.f, 25.f};
    p_verticies[4].position = {775.f, 75.f};
    p_verticies[5].position = {775.f, 25.f};
}

void TextureSelectionManager::updateSelectionPreview(int nTex)
{
    int row = std::floor(nTex/18); // 18 columns
    int column = std::floor(nTex%18);

    sf::Vector2f textureTL {column * 17.f, row * 17.f}; // you have 17 bc its tile size + spacing (16 + 1)
    sf::Vector2f textureBL {textureTL.x, textureTL.y + 16};
    sf::Vector2f textureTR {textureTL.x + 16, textureTL.y};
    sf::Vector2f textureBR {textureTL.x + 16, textureTL.y + 16};

    p_verticies[0].texCoords = textureTL;
    p_verticies[1].texCoords = textureBL;
    p_verticies[2].texCoords = textureBR;

    p_verticies[3].texCoords = textureTL;
    p_verticies[4].texCoords = textureBR;
    p_verticies[5].texCoords = textureTR;
}

void TextureSelectionManager::registerTextureGroup(TextureGroup new_texture_group, bool is_overlay)
{
    if(is_overlay)
    {
        overlayTextureGroups.push_back(new_texture_group);
    }else {
        textureGroups.push_back(new_texture_group);
    }
}

void TextureSelectionManager::setOverlayMode(bool overlayMode)
{
    tsm_overlayMode = overlayMode;

    if (overlayMode)
    {
        updateSelectionPreview(overlayTextureGroups[currOverlayGroup].s_textureIDs[currOverlayTextureIndex]);
        texGroupText->setString("Current Texture Group: " + overlayTextureGroups[currOverlayGroup].s_name);
    }else {
        updateSelectionPreview(textureGroups[currGroup].s_textureIDs[currTextureIndex]);  
        texGroupText->setString("Current Texture Group: " + textureGroups[currGroup].s_name); 
        
    }
}

void TextureSelectionManager::scrollTextureGroup(int factor)
{
    int& _currGroup = tsm_overlayMode ? currOverlayGroup : currGroup;
    int& index = tsm_overlayMode ? currOverlayTextureIndex : currTextureIndex;
    auto& groups = tsm_overlayMode ? overlayTextureGroups : textureGroups;

    _currGroup += factor;

    if (_currGroup == groups.size())
    {
        _currGroup = 0;
    }else if (_currGroup == -1)
    {
        _currGroup = groups.size()-1;
    }

    index = 0;
    updateSelectionPreview(groups[_currGroup].s_textureIDs[index]);
    texGroupText->setString("Current Texture Group: " + groups[_currGroup].s_name);
}

void TextureSelectionManager::scrollTexture(int factor)
{
    auto& groups = tsm_overlayMode ? overlayTextureGroups : textureGroups;
    int& index = tsm_overlayMode ? currOverlayTextureIndex : currTextureIndex;
    int& groupIndex = tsm_overlayMode ? currOverlayGroup : currGroup;

    int texSelectionSize = groups[groupIndex].s_textureIDs.size();
    if(texSelectionSize == 0) return;

    index += factor;

    if (index == -1)
    {
        index = texSelectionSize-1;
    }else if (index == texSelectionSize)
    {
        index = 0;
    }


    std::cout << index << '\n';

    updateSelectionPreview(groups[groupIndex].s_textureIDs[index]);
}

int TextureSelectionManager::getCurrentTexture()
{
    if (tsm_overlayMode)
    {
        return overlayTextureGroups[currOverlayGroup].s_textureIDs[currOverlayTextureIndex];
    }else {
        return textureGroups[currGroup].s_textureIDs[currTextureIndex];
    }
}