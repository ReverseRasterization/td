#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class Button
{
    public:

        struct Texture
        {
            sf::Texture& textureTilemap;
            
        };

        std::function<void()> onClick;

        Button(sf::Vector2f position, sf::Vector2f size);

        void draw(sf::RenderWindow& window)
        {
            window.draw(rect);
        }

        void handleClick(sf::Vector2i pos);
        
    private:

        sf::RectangleShape rect;
        
        sf::Vector2f b_position;
        sf::Vector2f b_size;
        
};