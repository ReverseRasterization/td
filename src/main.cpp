#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "map.h"
#include "tsm.h"
#include "history.h"

#include "ui/button.h"
#include "ui/prompt.h"
#include "menu/menu.h"
#include "menu/mapconfig.h"

/*
    TODO LIST

    1. Customize Map & Tile Size
    2. Add Map Save & Upload System
*/

enum Pointers
{
    Default,
    Overlay,
    Delete
};

sf::Vector2f clampVector(sf::Vector2f vector, sf::Vector2f min_vector, sf::Vector2f max_vector);

void changePointer(sf::VertexArray& pointer, Pointers pointer_type);
void placeTile(bool overlayMode, Map& map, TextureSelectionManager& textureSelectionManager, HistoryManager& history,sf::Vector2f mouseWorld, int width, int height, int tileSize);
void deleteOverlayTile(Map& map, HistoryManager& history, sf::Vector2f mouseWorld, int width, int height, int tileSize);

int main()
{   
    // Window
    sf::Vector2u userDesktopSize = sf::VideoMode::getDesktopMode().size;
    sf::RenderWindow window(sf::VideoMode({800,800}), "TD Map Maker", sf::Style::Titlebar | sf::Style::Close);
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

    // Fonts
    sf::Font font;
    if (!font.openFromFile("assets/font.ttf"))
    {
        std::cerr << "Failed to load font!\n";
        return -1;
    }

    // UI Elements
    sf::Texture pointerSet;
    if(!pointerSet.loadFromFile("assets/pointers.png"))
    {
        std::cerr << "Failed to load pointer tileset!";
        return -1;
    }

    sf::Texture buttonSet;
    if(!buttonSet.loadFromFile("assets/uicons.png"))
    {
        std::cerr << "Failed to load uicons tileset!";
        return -1;
    }

    sf::VertexArray highlighting(sf::PrimitiveType::Triangles, 6);
    changePointer(highlighting, Pointers::Default);

    sf::Text fpsText(font);
    fpsText.setString("FPS: 0");
    fpsText.setCharacterSize(24);
    fpsText.setPosition({10.f, 766.f});
    fpsText.setFillColor(sf::Color::Red);
    fpsText.setStyle(sf::Text::Bold);

    sf::Text overlayModeText(font);
    overlayModeText.setString("O");
    overlayModeText.setCharacterSize(12);
    overlayModeText.setPosition({720.f, 24.5f});
    overlayModeText.setFillColor(sf::Color::Blue);
    overlayModeText.setStyle(sf::Text::Bold || sf::Text::Underlined);

    sf::Text deleteModeText(font);
    deleteModeText.setString("D");
    deleteModeText.setCharacterSize(12);
    deleteModeText.setPosition({721.5f, 38.5f});
    deleteModeText.setFillColor(sf::Color::Red);
    deleteModeText.setStyle(sf::Text::Bold || sf::Text::Underlined);

    sf::SoundBuffer buffer ("assets/noti.mp3");
    Prompt prompt(Prompt::Type::Normal, "Test", buffer, font);

    // Texture Selection Manager Setup
    TextureSelectionManager textureSelectionManager(tileset);
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
    bool overlayMode {false};
    bool deleteMode {false};
    bool showOverlay {true};

    HistoryManager history(&map, &overlayMode);

    // Variables for the menu
    MapConfig mapConfig(font, map);

    Menu menu(&buttonSet, history, mapConfig);

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

                if (mousePos.y > 75){
                    if (rightDown)
                    {
                        float xDiff = (mousePos.x - initialMousePos.x) * zoomFactor;
                        float yDiff = (mousePos.y - initialMousePos.y) * zoomFactor;

                        sf::Vector2f delta = {xDiff, yDiff};

                        camera.setCenter(initialWorldPos - delta);
                    }else if (leftDown)
                    {
                        if (deleteMode)
                        {
                            deleteOverlayTile(map, history, mouseWorld, width, height, tileSize);
                        }else {
                            placeTile(overlayMode, map, textureSelectionManager, history, mouseWorld, width, height, tileSize);
                        }
                        
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

                
            }

            if (event->is<sf::Event::MouseButtonPressed>())
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (mousePos.y > 75 && !mapConfig.isActive()) {
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) // user wanna move the camera
                    {
                        rightDown = true;
                        initialMousePos = mousePos;
                        initialWorldPos = camera.getCenter();
                    }else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) { // user wanna build or something
                        
                        leftDown = true;

                        if (deleteMode)
                        {
                            deleteOverlayTile(map, history, window.mapPixelToCoords(mousePos), width, height, tileSize);
                        }else {
                            
                            placeTile(overlayMode, map, textureSelectionManager, history, window.mapPixelToCoords(sf::Mouse::getPosition(window)), width, height, tileSize);
                        }
                    }
                }else if (mousePos.y < 75){ 
                    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) { menu.handleClick(mousePos); }
                }

                if (mapConfig.isActive())
                {
                    mapConfig.handleClick(mousePos);
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
        
            if(const auto* textEntered = event->getIf<sf::Event::TextEntered>())
            {

                if (mapConfig.isActive() && textEntered->unicode < 128)
                {
                    mapConfig.handleChar(textEntered->unicode);
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
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ? history.redo() : history.undo();
                }
            
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O)) // Overlay Mode Toggle
                {
                    if (overlayMode)
                    {
                        overlayMode = false;
                        deleteMode = false;

                        changePointer(highlighting, Pointers::Default);
                    }else {
                        overlayMode = true;
                        changePointer(highlighting, Pointers::Overlay);
                    }

                    textureSelectionManager.setOverlayMode(overlayMode);
                }
            
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) && overlayMode) // Delete Mode Toggle
                {
                    if (deleteMode)
                    {
                        deleteMode = false;
                        changePointer(highlighting, Pointers::Overlay);
                    }else {
                        deleteMode = true;
                        changePointer(highlighting, Pointers::Delete);
                    }
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
        window.draw(highlighting, &pointerSet);
        
        window.setView(uiView);
        window.draw(fpsText);
        
        menu.draw(window);
        window.draw(textureSelectionManager);
        if (overlayMode){ window.draw(overlayModeText);}
        if (deleteMode){ window.draw(deleteModeText);}
        if (mapConfig.isActive()){mapConfig.draw(window);}

        prompt.draw(window);
        
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

void placeTile(bool overlayMode, Map& map, TextureSelectionManager& textureSelectionManager, HistoryManager& history,sf::Vector2f mouseWorld, int width, int height, int tileSize)
{
    if (mouseWorld.x < 0 || mouseWorld.x > width * tileSize || mouseWorld.y < 0 || mouseWorld.y > height * tileSize) {return;}

    int currTex = textureSelectionManager.getCurrentTexture();
    int tileID = map.getTileFromPosition(mouseWorld);
    int oldTex = map.getTileTexture(tileID, overlayMode);

    std::cout << "To place:\n   Old Texture: " << oldTex << "\n     New Texture: " << currTex << '\n';

    if (oldTex == currTex) {return;}

    map.changeTileTexture(tileID, overlayMode, currTex);
    history.recordAction(tileID, oldTex, currTex);
}

void deleteOverlayTile(Map& map, HistoryManager& history, sf::Vector2f mouseWorld, int width, int height, int tileSize)
{
    if (mouseWorld.x < 0 || mouseWorld.x > width * tileSize || mouseWorld.y < 0 || mouseWorld.y > height * tileSize) {return;}

    int tileID = map.getTileFromPosition(mouseWorld);
    int currentTex = map.getTileTexture(tileID, true);

    if (currentTex == 0) {return;}

    map.changeTileTexture(tileID, true, 0);

    history.recordAction(tileID, currentTex, 0);
}

void changePointer(sf::VertexArray& pointer, Pointers pointer_type)
{
    sf::Vector2f TL;
    switch(pointer_type)
    {
        case Default:
            TL = {0.f, 0.f};
            break;
        case Overlay:
            TL = {17.f, 0.f};
            break;
        case Delete:
            TL = {34.f, 0.f};
            break;
    }

    pointer[0].texCoords = TL; // Top-Left
    pointer[1].texCoords = {TL.x, 16.f}; // Bottom-Left
    pointer[2].texCoords = {TL.x + 16.f, TL.y + 16.f}; // Bottom-Right

    pointer[3].texCoords =  TL; // Top-Left
    pointer[4].texCoords = {TL.x + 16.f, TL.y + 16.f}; // Bottom-Right
    pointer[5].texCoords = {TL.x + 16.f, TL.y}; // Top-Right
}