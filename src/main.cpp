#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

#include <SFML/Graphics.hpp>
#include "map.h"
#include "tsm.h"

// TODO: Add Delete Mode For Overlays
// TODO: Add different pointers


struct Action
{
    int tileID;
    int oldTex, newTex;

    bool overlay;
};

sf::Vector2f clampVector(sf::Vector2f vector, sf::Vector2f min_vector, sf::Vector2f max_vector);
void placeTile(bool overlayMode, Map& map, TextureSelectionManager& textureSelectionManager, std::vector<Action>& undoStack, std::vector<Action>& redoStack,sf::Vector2f mouseWorld, int width, int height, int tileSize);

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
    constexpr int width {25};
    constexpr int height {25};
    constexpr std::size_t size = width * height;
    int tileSize {16};

    camera.setCenter({static_cast<float>((width*tileSize)/2), static_cast<float>((height*tileSize)/2)});

    sf::Texture tileset;
    if(!tileset.loadFromFile("assets/tilemap.png"))
    {
        std::cerr << "Failed to load tileset!\n";
        return -1;
    }

    Map map(tileset);

    std::vector<Map::Tile> level;

    for (int i = 0; i < (width * height); i++)
    {
        level.push_back(Map::Tile(2, 0, false));
    }

    map.load(level, width, height, tileSize);

    // UI Elements
    sf::Font font;
    if (!font.openFromFile("assets/font.ttf"))
    {
        std::cerr << "Failed to load font!\n";
        return -1;
    }

    sf::VertexArray highlighting(sf::PrimitiveType::Triangles, 6);

    highlighting[0].texCoords = {119.f, 51.f}; // Top-Left
    highlighting[1].texCoords = {119.f, 67.f}; // Bottom-Left
    highlighting[2].texCoords = {135.f, 67.f}; // Bottom-Right

    highlighting[3].texCoords = {119.f, 51.f}; // Top-Left
    highlighting[4].texCoords = {135.f, 67.f}; // Bottom-Right
    highlighting[5].texCoords = {135.f, 51.f}; // Top-Right

    sf::Text texGroupText(font);
    texGroupText.setString("Current Texture Group: Grass");
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

    sf::Text overlayModeText(font);
    overlayModeText.setString("Overlay Mode");
    overlayModeText.setCharacterSize(24);
    overlayModeText.setPosition({10.f, 49.f});
    overlayModeText.setFillColor(sf::Color::Blue);
    overlayModeText.setStyle(sf::Text::Bold || sf::Text::Underlined);

    // Texture Selection Manager Setup
    TextureSelectionManager textureSelectionManager(tileset, texGroupText);
    textureSelectionManager.registerTextureGroup(TextureSelectionManager::TextureGroup("Grass", {0,1,2}), false);
    textureSelectionManager.registerTextureGroup(TextureSelectionManager::TextureGroup("Water", {37}), false);
    textureSelectionManager.registerTextureGroup(TextureSelectionManager::TextureGroup("Horizontal Roads", {109, 110, 111}), true);
    textureSelectionManager.registerTextureGroup(TextureSelectionManager::TextureGroup("Vertical Roads", {126, 144, 162}), true);
    textureSelectionManager.registerTextureGroup(TextureSelectionManager::TextureGroup("Curved Roads", {127, 129, 163, 165}), true);

    // Variables for camera movement
    bool leftDown = false;
    bool rightDown = false;
    sf::Vector2i initialMousePos;
    sf::Vector2f initialWorldPos;

    // Variables for zoom
    sf::Vector2f baseSize = camera.getSize();
    sf::Vector2f minSize = baseSize/3.f; // how much the user can zoom in
    sf::Vector2f maxSize = baseSize*2.f; // how much the user can zoom out
    float zoomStep = 0.15f;
    float zoomFactor = 1.f;

    // Variables for building
    std::vector<Action> undoStack;
    std::vector<Action> redoStack;

    bool overlayMode {false};
    bool showOverlay {true};

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

            // Camera Movement & Block Highlighting & Placing
            if (event->is<sf::Event::MouseMoved>()) 
            {
                auto mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePos);

                if (rightDown)
                {
                    float xDiff = (mousePos.x - initialMousePos.x) * zoomFactor;
                    float yDiff = (mousePos.y - initialMousePos.y) * zoomFactor;

                    sf::Vector2f delta = {xDiff, yDiff};

                    camera.setCenter(initialWorldPos - delta);
                }else if (leftDown)
                {
                    placeTile(overlayMode, map, textureSelectionManager, undoStack, redoStack, mouseWorld, width, height, tileSize);
                }

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
                    rightDown = true;
                    initialMousePos = sf::Mouse::getPosition(window);
                    initialWorldPos = camera.getCenter();
                }else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) { // user wanna build
                    leftDown = true;
                    placeTile(overlayMode, map, textureSelectionManager, undoStack, redoStack, window.mapPixelToCoords(sf::Mouse::getPosition(window)), width, height, tileSize);
                }
            }

            if (event->is<sf::Event::MouseButtonReleased>())
            {
                if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
                {
                    rightDown = false;
                }

                if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                {
                    leftDown = false;
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
                        textureSelectionManager.scrollTextureGroup(-1);
                    }else {
                        textureSelectionManager.scrollTextureGroup(1);
                    }                    
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) // Go backward in texture group
                {
                    textureSelectionManager.scrollTexture(-1);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) // Go forward in texture group
                {
                    textureSelectionManager.scrollTexture(1);
                }
            
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) // Undo & Redo
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) && !redoStack.empty()) // redo
                    {
                        Action item = redoStack.back();
                        redoStack.pop_back();
                        undoStack.push_back(item);
                        
                        map.changeTileTexture(item.tileID, overlayMode, item.newTex);
                    }
                    
                    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) && !undoStack.empty()) { // undo
                        Action item = undoStack.back();
                        undoStack.pop_back();
                        redoStack.push_back(item);

                        map.changeTileTexture(item.tileID, overlayMode, item.oldTex); 
                    }
                }
            
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O)) // Overlay Mode Toggle
                {
                    if (overlayMode)
                    {
                        overlayMode = false;
                    }else {
                        overlayMode = true;
                    }

                    textureSelectionManager.setOverlayMode(overlayMode);
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
        window.draw(textureSelectionManager);
        window.draw(fpsText);
        if (overlayMode){ window.draw(overlayModeText);}

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

void placeTile(bool overlayMode, Map& map, TextureSelectionManager& textureSelectionManager, std::vector<Action>& undoStack, std::vector<Action>& redoStack,sf::Vector2f mouseWorld, int width, int height, int tileSize)
{
    if (mouseWorld.x < 0 || mouseWorld.x > width * tileSize || mouseWorld.y < 0 || mouseWorld.y > height * tileSize) {return;}

    int currTex = textureSelectionManager.getCurrentTexture();
    int tileID = map.getTileFromPosition(mouseWorld);
    int oldTex = map.getTileTexture(tileID, overlayMode);

    if (oldTex == currTex) {return;}

    map.changeTileTexture(tileID, overlayMode, currTex);

    Action nAction {
        .tileID = tileID,
        .oldTex = oldTex,
        .newTex = currTex,
        .overlay = overlayMode
    };

    
    if (undoStack.size() == 500)
    {
        undoStack = {};
    }
    undoStack.push_back(nAction);
    redoStack = {};
}