#include "prompt.h"

Prompt::Prompt(Type prompt_type, std::string text, sf::SoundBuffer& displayBuffer, sf::Font& font)
    :   m_displaySound(displayBuffer),
        frame({500.f, 200.f}),
        promptText(font, text, 30)
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
        default:
            frame.setFillColor(sf::Color(125, 152, 161));
            frame.setOutlineColor(sf::Color(28, 35, 33));
            break;
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