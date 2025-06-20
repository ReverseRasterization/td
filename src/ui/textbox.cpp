#include "textbox.h"
#include <iostream>

void Textbox::changePos(sf::Vector2f pos)
{
    background.setPosition(pos);
    text.setPosition(background.getPosition() + sf::Vector2f(background.getSize().x*.1, (background.getSize().y-charSize)/2));
}

void Textbox::handleClick(sf::Vector2f pos)
{
    if (background.getGlobalBounds().contains(pos))
    {
        selected = true;

        background.getSize().x > background.getSize().y ? 
            background.setOutlineThickness(background.getSize().x*.1) : background.setOutlineThickness(background.getSize().y*.1);

        background.setOutlineColor(sf::Color::Black);
    }else {
        selected = false;

        background.setOutlineThickness(0.f);
    }
}

void Textbox::handleChar(char32_t character)
{
    if (!selected || (character != 8 && text.getString().getSize() >= maxChar)){return;}

    
    if (character == 8) // backspace
    {
        if (!currString.empty())
        {
            currString.pop_back();
        }
    }else {
        if (m_rule == ANY)
        {
            currString += character;
        }else if (m_rule == NUMBERS_ONLY)
        {
            if (character >= 48 && character <= 57)
            {
                currString += character;
            }
        }else // letters only
        {
            if ((character >= 65 && character <= 90) || (character >= 97 && character <= 122))
            {
                currString += character;
            }
        }
        
    }

    text.setString(currString);
}

void Textbox::setString(std::string new_string)
{
    currString = new_string;
    text.setString(currString);
}