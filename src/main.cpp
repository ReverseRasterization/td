#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode({800,800}), "My window");

    sf::VertexArray tri(sf::PrimitiveType::Triangles, 3);

    tri[0].position = sf::Vector2f(10.f, 10.f);
    tri[1].position = sf::Vector2f(10.f, 200.f);
    tri[2].position = sf::Vector2f(200.f, 200.f);

    tri[0].color = sf::Color::Red;
    tri[1].color = sf::Color::Blue;
    tri[2].color = sf::Color::Green;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();
        window.draw(tri);
        window.display();
    }

    return 0;
}