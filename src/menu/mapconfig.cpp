#include "mapconfig.h"
#include <iostream>

MapConfig::MapConfig(sf::Font& font):
    frame(sf::Vector2f(500.f, 200.f)),
    title(font, "Map Configuration", 20),
    sizeTitle(font, "Width x Height", 15),

    widthTB(font, {50.f, 25.f}, 15),
    heightTB(font, {50.f, 25.f}, 15),

    applyButtion({0.f, 0.f}, {100.f, 25.f}, Button::Text("Apply", &font, 15, sf::Color::Black), std::nullopt, sf::Color::Green)
{
    frame.setPosition({150.f, 300.f});
    frame.setFillColor(sf::Color(4, 53, 101));
    frame.setOutlineThickness(15.f);
    frame.setOutlineColor(sf::Color(2, 57, 74));

    sf::FloatRect bounds = title.getLocalBounds();
    title.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
    
    title.setFillColor(sf::Color::White);
    title.setPosition(frame.getPosition() + sf::Vector2f(frame.getSize().x/2, 0));

    bounds = sizeTitle.getLocalBounds();
    sizeTitle.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
    
    sizeTitle.setFillColor(sf::Color::White);
    sizeTitle.setPosition(frame.getPosition() + sf::Vector2f(frame.getSize().x/2, frame.getSize().y/2) - sf::Vector2f(0.f, 25.f));

    widthTB.setPosition(frame.getPosition() + sf::Vector2f(frame.getSize().x/2, frame.getSize().y/2) - sf::Vector2f(55.f, 12.5f));
    widthTB.setMaxCharacters(3);

    heightTB.setPosition(frame.getPosition() + sf::Vector2f(frame.getSize().x/2, frame.getSize().y/2) - sf::Vector2f(-5.f, 12.5f));
    heightTB.setMaxCharacters(3);

    applyButtion.setPosition(frame.getPosition() + sf::Vector2f(frame.getSize().x/2, frame.getSize().y/2) + sf::Vector2f(-50.f, 25.f));
}

void MapConfig::handleClick(sf::Vector2i position)
{
    widthTB.handleClick(static_cast<sf::Vector2f>(position));
    heightTB.handleClick(static_cast<sf::Vector2f>(position));

    applyButtion.handleClick(position);
}

void MapConfig::handleChar(char32_t character)
{
    widthTB.handleChar(character);
    heightTB.handleChar(character);
}