#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

#include <SFML/Graphics.hpp>
#include "map.h"

// TODO: Make it so that placing attempts cannot be made outside of the map
// TODO: Revamp undo system to include redo and history limit
// TODO: Make it so that a user can hold down the mouse to place blocks
// TODO: Add overlay system

struct TextureGroup
{
    std::string s_name;
    std::vector<int> s_textureIDs;

    TextureGroup(std::string name, std::vector<int> textureIDs): s_name(name), s_textureIDs(textureIDs){};
};

sf::Vector2f clampVector(sf::Vector2f vector, sf::Vector2f min_vector, sf::Vector2f max_vector);

void updateSelectionPreview(int nTex, sf::Texture& tilemap, sf::VertexArray& va)
{
    int row = std::floor(nTex/18); // 18 columns
    int column = std::floor(nTex%18);

    sf::Vector2f textureTL {column * 17.f, row * 17.f}; // you have 17 bc its tile size + spacing (16 + 1)
    sf::Vector2f textureBL {textureTL.x, textureTL.y + 16};
    sf::Vector2f textureTR {textureTL.x + 16, textureTL.y};
    sf::Vector2f textureBR {textureTL.x + 16, textureTL.y + 16};

    va[0].texCoords = textureTL;
    va[1].texCoords = textureBL;
    va[2].texCoords = textureBR;

    va[3].texCoords = textureTL;
    va[4].texCoords = textureBR;
    va[5].texCoords = textureTR;
}

int main()
{   
    // Window
    sf::Vector2u userDesktopSize = sf::VideoMode::getDesktopMode().size;
    sf::RenderWindow window(sf::VideoMode({800,800}), "My window", sf::Style::Titlebar | sf::Style::Close);
    window.setPosition({static_cast<int>((userDesktopSize.x/2)-400), static_cast<int>((userDesktopSize.y/2)-400)});

    // Camera / Views
    sf::View camera;
    camera.setSize({800,800});

    sf::View uiView;
    uiView.setCenter({400,400});
    uiView.setSize({800, 800});

    // Map Setup
    constexpr int width {100};
    constexpr int height {500};
    constexpr std::size_t size = width * height;
    int tileSize {10};

    camera.setCenter({static_cast<float>((width*tileSize)/2), static_cast<float>((height*tileSize)/2)});

    sf::Texture tileset;
    if(!tileset.loadFromFile("assets/tilemap.png"))
    {
        std::cerr << "Failed to load tileset!\n";
        return -1;
    }

    Map map(tileset);
    std::array<int, size> level;
    for (int& tile : level) {
        tile = 2;
    }

    if (map.load(level.data(), width, height, tileSize))
    {   
        std::cout << "Loaded!\n";
    }else {
        std::cerr << "Failed to load map!";
        return -1; 
    }

    // UI Elements
    sf::RectangleShape box({100.f, 100.f});
    box.setPosition({700.f, 0.f});

    sf::VertexArray preview(sf::PrimitiveType::Triangles, 6);

    preview[0].position = {725.f, 25.f};
    preview[1].position = {725.f, 75.f};
    preview[2].position = {775.f, 75.f};

    preview[3].position = {725.f, 25.f};
    preview[4].position = {775.f, 75.f};
    preview[5].position = {775.f, 25.f};

    updateSelectionPreview(0, tileset, preview);

    sf::VertexArray highlighting(sf::PrimitiveType::Triangles, 6);

    highlighting[0].texCoords = {119.f, 51.f}; // Top-Left
    highlighting[1].texCoords = {119.f, 67.f}; // Bottom-Left
    highlighting[2].texCoords = {135.f, 67.f}; // Bottom-Right

    highlighting[3].texCoords = {119.f, 51.f}; // Top-Left
    highlighting[4].texCoords = {135.f, 67.f}; // Bottom-Right
    highlighting[5].texCoords = {135.f, 51.f}; // Top-Right

    sf::Font font;
    if (!font.openFromFile("assets/font.ttf"))
    {
        std::cerr << "Failed to load font!\n";
        return -1;
    }

    sf::Text texGroupText(font);
    texGroupText.setString("Current Texture Group: Grasses");
    texGroupText.setCharacterSize(24);
    texGroupText.setPosition({10.f, 10.f});
    texGroupText.setFillColor(sf::Color::Red);
    texGroupText.setStyle(sf::Text::Bold);

    sf::Text fpsText(font);
    fpsText.setString("FPS: 0");
    fpsText.setCharacterSize(24);
    fpsText.setPosition({10.f, 766.f});
    fpsText.setFillColor(sf::Color::Red);
    fpsText.setStyle(sf::Text::Bold);

    // Variables for camera movement
    bool subscribed = false;
    sf::Vector2i initialMousePos;
    sf::Vector2f initialWorldPos;

    // Variables for zoom
    sf::Vector2f baseSize = camera.getSize();
    sf::Vector2f minSize = baseSize/3.f; // how much the user can zoom in
    sf::Vector2f maxSize = baseSize*2.f; // how much the user can zoom out
    float zoomStep = 0.15f;
    float zoomFactor = 1.f;

    // Variables for building
    int currGroup{0};
    int currTextureIndex{0};
    std::vector<TextureGroup> textureGroups {
        TextureGroup("Grass", {0,1,2}),
        TextureGroup("Horizontal Roads", {109, 110, 111}),
        TextureGroup("Vertical Roads", {126, 144, 162}),
        TextureGroup("Curved Roads", {127, 129, 163, 165}),
    };

    std::vector<std::vector<int>> history; // each item: {tileID, textureID}

    // FPS Variables

    sf::Clock clock;    
    int fps {0};
    int i {0};
    

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // Camera Movement & Block Highlighting
            if (event->is<sf::Event::MouseMoved>()) 
            {
                auto mousePos = sf::Mouse::getPosition(window);

                if (subscribed)
                {
                    float xDiff = (mousePos.x - initialMousePos.x) * zoomFactor;
                    float yDiff = (mousePos.y - initialMousePos.y) * zoomFactor;

                    sf::Vector2f delta = {xDiff, yDiff};

                    camera.setCenter(initialWorldPos - delta);
                }

                sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePos);

                float x = tileSize * (std::floor(mouseWorld.x/tileSize));
                float y = tileSize * (std::floor(mouseWorld.y/tileSize));

                highlighting[0].position = {x, y}; // Top-Left
                highlighting[1].position = {x, y+tileSize}; // Bottom-Left
                highlighting[2].position = {x+tileSize, y+tileSize}; // Bottom Right

                highlighting[3].position = {x, y}; // Top Left
                highlighting[4].position = {x+tileSize, y+tileSize}; // Bottom right
                highlighting[5].position = {x+tileSize, y}; // Top Right
                
                
            }

            if (event->is<sf::Event::MouseButtonPressed>())
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) // user wanna move the camera
                {
                    subscribed = true;
                    initialMousePos = sf::Mouse::getPosition(window);
                    initialWorldPos = camera.getCenter();
                }else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) { // user wanna build
                    sf::Vector2f mouseWorld = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                    int tile = map.getTileFromPosition(mouseWorld);
                    int currTex = textureGroups[currGroup].s_textureIDs[currTextureIndex];

                    if (map.getTileTexture(tile) != currTex) {
                        history.push_back({tile, map.getTileTexture(tile)});

                        map.changeTileTexture(tile, currTex);
                    }

                    
                }
                
            }

            if (event->is<sf::Event::MouseButtonReleased>())
            {
                if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
                {
                    subscribed = false;
                }
                
            }

            // Zoom
            if (const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                if(mouseWheelScrolled->wheel == sf::Mouse::Wheel::Vertical)
                {
                    sf::Vector2f cameraSize = camera.getSize();

                    if(mouseWheelScrolled->delta == 1) { // up
                        camera.setSize(clampVector(cameraSize*(1.f-zoomStep), minSize, maxSize));
                    }else if (mouseWheelScrolled->delta == -1) { // down
                        camera.setSize(clampVector(cameraSize*(1.f+zoomStep), minSize, maxSize));
                    }

                    zoomFactor = cameraSize.x / baseSize.x;
                }
            }
        
            // Block Selection & Build Mode Toggling
            if (event->is<sf::Event::KeyPressed>())
            {
            
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::G)) // Change Texture Groups
                {
                    int factor;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt))
                    {
                        factor = -1;
                    }else {
                        factor = 1;
                    }

                    currGroup += factor;

                    if (currGroup == textureGroups.size())
                    {
                        currGroup = 0;
                    }else if (currGroup == -1){
                        currGroup = textureGroups.size()-1;
                    }

                    currTextureIndex = 0;

                    updateSelectionPreview(textureGroups[currGroup].s_textureIDs[currTextureIndex], tileset, preview);  
                    texGroupText.setString("Current Texture Group: " + textureGroups[currGroup].s_name);  
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) // Go backward in texture group
                {
                    int texSelectionSize = textureGroups[currGroup].s_textureIDs.size();

                    if (currTextureIndex == 0)
                    {
                        currTextureIndex = texSelectionSize-1;
                    }else {
                        currTextureIndex--;
                    }

                    updateSelectionPreview(textureGroups[currGroup].s_textureIDs[currTextureIndex], tileset, preview); 
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) // Go forward in texture group
                {
                    int texSelectionSize = textureGroups[currGroup].s_textureIDs.size();

                    if (currTextureIndex == texSelectionSize-1)
                    {
                        currTextureIndex = 0;
                    }else {
                        currTextureIndex++;
                    }

                    updateSelectionPreview(textureGroups[currGroup].s_textureIDs[currTextureIndex], tileset, preview); 
                }
            
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) && !history.empty())
                {
                    std::vector<int> item = history.back();
                    history.pop_back();

                    map.changeTileTexture(item[0], item[1]);
                }
            }
        }

        float time = clock.getElapsedTime().asSeconds();
        i++;

        if (time > 1.f)
        {

            clock.restart();
            fps = std::floor(i/time);

            i = 0;
        }

        fpsText.setString("FPS: " + std::to_string(fps));

        window.clear();

        window.setView(camera);
        window.draw(map);
        window.draw(highlighting, &tileset);
        
        window.setView(uiView);
        window.draw(box);
        window.draw(preview, &tileset);
        window.draw(texGroupText);
        window.draw(fpsText);

        window.setView(camera);

        window.display();
    }

    return 0;
}

sf::Vector2f clampVector(sf::Vector2f vector, sf::Vector2f min_vector, sf::Vector2f max_vector)
{
    float x = std::clamp(vector.x, min_vector.x, max_vector.x);
    float y = std::clamp(vector.y, min_vector.y, max_vector.y);

    return {x,y};
}