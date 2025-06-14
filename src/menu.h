#pragma once

#include <vector>

#include <SFML/Graphics.hpp>
#include "ui/button.h"
#include "history.h"

class Menu
{
    public:
        void draw(sf::RenderWindow& window) {
            window.draw(background);
            undoButton.draw(window);
            redoButton.draw(window);
        }

        void handleClick(sf::Vector2i mousePos);

        Menu(sf::Texture* uicons, HistoryManager& history);
    private:

        HistoryManager& history;

        sf::RectangleShape background;

        Button undoButton = Button({12.5f,12.5f}, {50.f, 50.f});
        Button redoButton = Button({62.5f,12.5f}, {50.f, 50.f});
};