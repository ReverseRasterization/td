#pragma once

#include <SFML/Graphics.hpp>

class Textbox
{
    public:

        enum Rule
        {
            NUMBERS_ONLY,
            LETTERS_ONLY,
            ANY
        };

        Textbox(sf::Font& font, sf::Vector2f size, unsigned int char_size)
        :   background(size),
            text(font, "", char_size),
            charSize(char_size)
        {
            changePos({0.f, 0.f});
            text.setFillColor(sf::Color::Black);
        }

        void setPosition(sf::Vector2f position){changePos(position);}
        sf::Vector2f getPosition(){return background.getPosition();}

        void setMaxCharacters(unsigned int max_characters) {maxChar = max_characters;}
        void setRule(Rule rule) {m_rule = rule;};

        void handleClick(sf::Vector2f pos);
        void handleChar(char32_t character);

        void setString(std::string new_string);
        std::string getString(){return currString;};

        void draw(sf::RenderWindow& window)
        {
            window.draw(background);
            window.draw(text);
        }

    private:

        Rule m_rule {ANY};

        sf::RectangleShape background;
        sf::Text text;

        unsigned int charSize;
        unsigned int maxChar {69420};

        bool selected {false};

        std::string currString = "";

        void changePos(sf::Vector2f pos);

};