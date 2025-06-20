#pragma once

#include <SFML/Graphics.hpp>

#include "../ui/textbox.h"
#include "../ui/button.h"

#include "../map.h"

class MapConfig
{
    public:

        bool isActive(){return active;}

        void open();
        void close(){active = false;}

        void handleClick(sf::Vector2i position);
        void handleChar(char32_t character);

        void draw(sf::RenderWindow& window)
        {
            window.draw(frame);
            window.draw(title);
            window.draw(sizeTitle);

            widthTB.draw(window);
            heightTB.draw(window);
            applyButtion.draw(window);
        }

        MapConfig(sf::Font& font, Map& map);

    private:

        Map& map;

        sf::RectangleShape frame;

        sf::Text title;
        sf::Text sizeTitle;

        Textbox widthTB;
        Textbox heightTB;

        Button applyButtion;

        bool active{false};

        void changeMapProperties();
};