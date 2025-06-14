#pragma once

#include <vector>

#include <SFML/Graphics.hpp>
#include "ui/button.h"

class Menu
{
    public:
        void draw(sf::RenderWindow& window) {
            window.draw(background);
            mapPropertiesButton.draw(window);
        }


        Menu();
    private:

        sf::RectangleShape background;

        Button mapPropertiesButton = Button({12.5f,12.5f}, {50.f, 50.f});

        std::vector<Button> menuButtons;
};