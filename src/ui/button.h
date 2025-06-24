#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>

class Button
{
    public:
        struct Text
        {
            std::string t_text;
            unsigned int t_font_size;

            sf::Font* t_font;
            std::unique_ptr<sf::Text> t_txt;

            Text(std::string text, sf::Font* font, unsigned int font_size=24, sf::Color text_color=sf::Color::Black, sf::Color outline_color=sf::Color::Black)
            {
                sf::Text nText(*font, text, font_size);
                nText.setFillColor(text_color);
                nText.setOutlineColor(outline_color);

                t_txt = std::make_unique<sf::Text>(nText);
            }
        };

        struct Texture
        {
            sf::Texture* t_texture;

            sf::Vector2i t_tile_position;
            sf::Vector2i t_tile_size;

            Texture(sf::Texture* texture, sf::Vector2i tile_position, sf::Vector2i tile_size): t_texture(texture), t_tile_position(tile_position), t_tile_size(tile_size) {}
        };

        bool clicked(sf::Vector2i mouse_pos);
        
        void setText(std::string new_text);
        void setTexture(std::optional<Texture> new_texture=std::nullopt, std::optional<sf::Vector2i> new_position=std::nullopt);

        int getState(){return state;}
        void setState(int new_state){state=new_state;}

        void setPosition(sf::Vector2f position);

        sf::Vector2f getSize() { return b_size;}
        //sf::Vector2f getPosition() {return b_position;}

        void draw(sf::RenderWindow& window)
        {
            if (btnTexture.has_value())
            {
                window.draw(rect);
            }else {
                window.draw(rect);
            }
            

            if (btnText.has_value())
            {
                window.draw(*btnText->t_txt.get());
            }
            
        }

        Button(sf::Vector2f position, sf::Vector2f size, std::optional<Text> button_text = std::nullopt, std::optional<Texture> button_texture = std::nullopt, std::optional<sf::Color> button_color = std::nullopt);
        
    private:

        sf::RectangleShape rect;
        std::optional<Text> btnText;
        sf::Font* font;

        std::optional<Texture> btnTexture;

        int state {0};
        
        sf::Vector2f b_position;
        sf::Vector2f b_size;
        
};