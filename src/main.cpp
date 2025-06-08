#include <iostream>
#include <algorithm>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "map.h"

sf::Vector2f clampVector(sf::Vector2f vector, sf::Vector2f min_vector, sf::Vector2f max_vector);

int main()
{
    sf::RenderWindow window(sf::VideoMode({800,800}), "My window", sf::Style::Titlebar | sf::Style::Close);
    sf::View camera;

    camera.setCenter({400,400});
    camera.setSize({800,800});

    Map map;
    if (map.load(10, 10, 30))
    {   
        std::cout << "Loaded!\n";
    }else {
        std::cerr << "Failed to load map!";
        return -1; 
    }

    // Variables for camera movement
    bool subscribed;
    sf::Vector2i initialMousePos;
    sf::Vector2f initialWorldPos;

    // Variables for zoom
    sf::Vector2f baseSize = camera.getSize();
    sf::Vector2f minSize = baseSize/2.f; // how much the user can zoom in
    sf::Vector2f maxSize = baseSize*2.f; // how much the user can zoom out
    float zoomStep = 0.15f;
    float zoomFactor = 1.f;
   
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
        }

        window.clear();
        window.setView(camera);
        window.draw(map);
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