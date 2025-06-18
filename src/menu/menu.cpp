#include "menu.h"
#include <iostream>

Menu::Menu(sf::Texture* uicons, HistoryManager& history, MapConfig& mapConfig): history(history), mapConfig(mapConfig)
{
    background.setSize({800.f, 75.f});
    background.setPosition({0.f, 0.f});
    background.setFillColor(sf::Color(59, 59, 59));

    undoButton.setTexture(Button::Texture(uicons, {0,0}, {32, 32}));
    redoButton.setTexture(Button::Texture(uicons, {32, 0}, {32, 32}));
    mapConfigButton.setTexture(Button::Texture(uicons, {64,0}, {32, 32}));

    undoButton.onClick = [&history](Button&)
    {
        history.undo();
    };

    redoButton.onClick = [&history](Button&)
    {
        history.redo();
    };

    mapConfigButton.onClick = [&mapConfig](Button&)
    {
        mapConfig.isActive() ? mapConfig.close() : mapConfig.open();
    };
}

void Menu::handleClick(sf::Vector2i mousePos)
{
    undoButton.handleClick(mousePos);
    redoButton.handleClick(mousePos);
    mapConfigButton.handleClick(mousePos);
}



