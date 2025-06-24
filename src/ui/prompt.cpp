#include "prompt.h"
#include <iostream>

Prompt::Prompt(Type prompt_type, std::string text, sf::SoundBuffer& displayBuffer, sf::Font& font)
    :   m_displaySound(displayBuffer),
        m_type(prompt_type),
        frame({500.f, 200.f}),
        promptText(font, text, 30),
        button1({0.f, 0.f}, {100.f, 25.f}, Button::Text("...", &font, 24)),
        button2({0.f, 0.f}, {100.f, 25.f}, Button::Text("...", &font, 24))
{
    frame.setOutlineThickness(15.f);
    frame.setPosition({150.f, 300.f});

    switch(prompt_type)
    {
        case Error:
            frame.setFillColor(sf::Color(107, 15, 26));
            frame.setOutlineColor(sf::Color(61, 8, 15));
            break;
        case Warning:
            frame.setFillColor(sf::Color(255, 111, 89));
            frame.setOutlineColor(sf::Color(219, 80, 74));
            break;
        default: // normal
            frame.setFillColor(sf::Color(125, 152, 161));
            frame.setOutlineColor(sf::Color(28, 35, 33));
            break;
    }

    sf::Vector2f frameSize = frame.getSize();
    sf::Vector2f framePosition = frame.getPosition();

    sf::Vector2f buttonSize = button1.getSize();

    if (prompt_type == Error || prompt_type == Normal) // "Just has OK"
    {
        button1.setText("Okay");
        button1.setPosition({frameSize.x/2 + (buttonSize.x), framePosition.y + (frameSize.y-35)});

        button1.onClick = [&](Button&)
        {
            onResponse(button1);
        };
    }else { // warning, has proceed and cancel
        
    }

    /*
        Error:
            Fill: (107, 15, 26)
            Outline: (61, 8, 15)

        Warning:
            Fill: (255, 111, 89)
            Outline: (219, 80, 74)

        Normal:
            Fill: (125, 152, 161)
            Outline: (28, 35, 33)
    */
}

void Prompt::onResponse(Button& button)
{
    std::cout << "Button Pressed! " << &button; 
}

void Prompt::handleClick(sf::Vector2i pos)
{
    
}