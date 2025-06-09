#include <iostream>
#include <algorithm>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "map.h"
#include <random>

sf::Vector2f clampVector(sf::Vector2f vector, sf::Vector2f min_vector, sf::Vector2f max_vector);

void updateSelectionPreview(int nTex, sf::Texture& tilemap, sf::VertexArray& va)
{
    std::cout << "Called!";

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
    camera.setCenter({400,400});
    camera.setSize({800,800});

    sf::View uiView;
    uiView.setCenter({400,400});
    uiView.setSize({800, 800});

    
    

    // Map Setup
    sf::Texture tileset;
    if(!tileset.loadFromFile("assets/tilemap.png"))
    {
        std::cerr << "Failed to load tileset!\n";
        return -1;
    }

    Map map(tileset);
    std::array<int, 2500> level;
    for (int& tile : level) {
        tile = 2;
    }
    if (map.load(level.data(), 50, 50, 16))
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

    // Variables for block selection
    int currTexture {0};
   
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // Camera Movement
            if (event->is<sf::Event::MouseMoved>() && subscribed) 
            {
                auto mousePos = sf::Mouse::getPosition(window);
                float xDiff = (mousePos.x - initialMousePos.x) * zoomFactor;
                float yDiff = (mousePos.y - initialMousePos.y) * zoomFactor;

                sf::Vector2f delta = {xDiff, yDiff};

                camera.setCenter(initialWorldPos - delta);
            }

            if (event->is<sf::Event::MouseButtonPressed>())
            {
                subscribed = true;
                initialMousePos = sf::Mouse::getPosition(window);
                initialWorldPos = camera.getCenter();
            }

            if (event->is<sf::Event::MouseButtonReleased>())
            {
                subscribed = false;
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
        
            // Block Selection
            if (event->is<sf::Event::KeyPressed>())
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
                {
                    if (currTexture == 198)
                    {
                        currTexture = 0;
                    }else {
                        currTexture++;
                    }
                }

                updateSelectionPreview(currTexture, tileset, preview);
            }
        }

        window.clear();

        window.setView(camera);
        window.draw(map);

        window.setView(uiView);
        window.draw(box);
        window.draw(preview, &tileset);

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