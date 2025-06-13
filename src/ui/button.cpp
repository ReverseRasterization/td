#include "button.h"

Button::Button(sf::Vector2f position, sf::Vector2f size): b_position(position), b_size(size)
{
    rect.setSize(size);
    rect.setPosition(position);
}

void Button::handleClick(sf::Vector2i pos)
{
    if (pos.x < b_position.x || pos.x > b_position.x + b_size.x || pos.y < b_position.y || pos.y > b_position.y + b_size.y) {return;}

    if(onClick){onClick();}
}