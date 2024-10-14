#include <iostream>

#include "Graph.hpp"


int main()
{
    std::ios_base::sync_with_stdio(0);
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "A*");

    bool canPlace{ true };
    astar::Graph::getInstance().setDrawDistance(true);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Vector2f mousePos(sf::Mouse::getPosition(window));

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (canPlace)
            {
                astar::Graph::getInstance().addNode(mousePos);
                canPlace = false;
            }
        }
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            astar::Graph::getInstance().checkAndDelete(mousePos);
        }
        else
        {
            canPlace = true;
        }

        window.clear();
        astar::Graph::getInstance().draw(window, mousePos);
        window.display();
    }
}
