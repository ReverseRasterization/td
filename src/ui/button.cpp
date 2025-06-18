#include "button.h"

Button::Button(sf::Vector2f position, sf::Vector2f size, std::optional<Button::Text> button_text, std::optional<Button::Texture> button_texture, std::optional<sf::Color> button_color): b_position(position), b_size(size)
{
    rect.setSize(size);
    rect.setPosition(position);

    if (button_text.has_value())
    {
        sf::FloatRect textBounds = button_text->t_txt->getLocalBounds();
        button_text->t_txt->setOrigin(textBounds.getCenter());

        button_text->t_txt.get()->setPosition({position.x + (size.x/2), position.y + (size.y / 2)});
        btnText = std::move(button_text);
    }else if (button_texture.has_value())
    {
        btnTexture = button_texture;

        rect.setTexture(button_texture->t_texture);
        rect.setTextureRect(sf::IntRect(button_texture->t_tile_position, button_texture->t_tile_size));
    }

    if (!button_texture.has_value() && button_color.has_value())
    {
        rect.setFillColor(button_color.value());
    }
}

void Button::handleClick(sf::Vector2i pos)
{
    if (pos.x < b_position.x || pos.x > b_position.x + b_size.x || pos.y < b_position.y || pos.y > b_position.y + b_size.y) {return;}

    if(onClick){onClick(*this);}
}

void Button::setText(std::string new_text)
{
    if(!btnText.has_value()){return;}

    btnText->t_txt->setString(new_text);
    sf::FloatRect textBounds = btnText->t_txt->getLocalBounds();
    btnText->t_txt->setOrigin(textBounds.getCenter());

    btnText->t_txt.get()->setPosition({b_position.x + (b_size.x/2), b_position.y + (b_size.y / 2)});
}

void Button::setTexture(std::optional<Texture> new_texture, std::optional<sf::Vector2i> new_position)
{
    if (new_texture.has_value())
    {
        btnTexture = new_texture;

        rect.setTexture(new_texture->t_texture);
        rect.setTextureRect(sf::IntRect(new_texture->t_tile_position, new_texture->t_tile_size));
    }else if (new_position.has_value())
    {   
        btnTexture->t_tile_position = new_position.value();
        rect.setTextureRect(sf::IntRect(btnTexture->t_tile_position, btnTexture->t_tile_size));
    }
}

void Button::setPosition(sf::Vector2f position)
{
    b_position = position;

    rect.setPosition(b_position);

    if (!btnText.has_value()){return;}

    btnText->t_txt.get()->setPosition({b_position.x + (b_size.x/2), b_position.y + (b_size.y / 2)});
}