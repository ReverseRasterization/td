#include "menu.h"

Menu::Menu(sf::Texture* uicons, HistoryManager& history): history(history)
{
    background.setSize({800.f, 75.f});
    background.setPosition({0.f, 0.f});
    background.setFillColor(sf::Color(59, 59, 59));

    undoButton.setTexture(Button::Texture(uicons, {0,0}, {32, 32}));
    redoButton.setTexture(Button::Texture(uicons, {32, 0}, {32, 32}));

    undoButton.onClick = [&history](Button&)
    {
        history.undo();
    };

    redoButton.onClick = [&history](Button&)
    {
        history.redo();
    };
}

void Menu::handleClick(sf::Vector2i mousePos)
{
    undoButton.handleClick(mousePos);
    redoButton.handleClick(mousePos);
}



