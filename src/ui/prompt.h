#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "button.h"

class Prompt
{
    public:

        enum Type
        {
            Error,
            Warning,
            Normal
        };

        enum Response
        {
            OK,
            PROCEED,
            CANCEL,
        };

        Prompt(Type prompt_type, std::string text, sf::SoundBuffer& displayBuffer, sf::Font& font);

        Response display();

        bool isVisible(){return visible;}

        void draw(sf::RenderWindow& window)
        {
            window.draw(frame);
            button1.draw(window);

            if (m_type == Warning)
            {
                button2.draw(window);
            }
        }

        void handleClick(sf::Vector2i pos);

    private:

        sf::Sound m_displaySound;

        sf::RectangleShape frame;
        sf::Text promptText;

        Type m_type;

        Button button1;
        Button button2;

        bool visible {false};

        void onResponse(Button& button);

};