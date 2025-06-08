// TOOD: Zoom Feature

#include <iostream>
#include <SFML/Graphics.hpp>
#include "map.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({800,800}), "My window");
    sf::View camera;

    camera.setCenter({400,400});
    camera.setSize({800,800});

    Map map;
    sf::RectangleShape indicator({50.f, 50.f});
    indicator.setPosition({750, 750});
    indicator.setFillColor(sf::Color::Red);

    if (map.load(10, 10, 30))
    {   
        std::cout << "Loaded!\n";
    }else {
        std::cerr << "Failed to load map!";
        return -1; 
    }

    bool subscribed;
    sf::Vector2i initialMousePos;
    sf::Vector2f initialWorldPos;
   

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (event->is<sf::Event::MouseMoved>() && subscribed) 
            {
                auto mousePos = sf::Mouse::getPosition(window);

                float xDiff = mousePos.x - initialMousePos.x;
                float yDiff = mousePos.y - initialMousePos.y;

                camera.setCenter({initialWorldPos.x-xDiff, initialWorldPos.y - yDiff});
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
        }

        window.clear();

        window.draw(indicator);

        window.setView(camera);
        window.draw(map);
        
        window.display();
    }

    return 0;
}