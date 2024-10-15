//test czy git działa

#include <iostream>

#include "Graph.hpp"


int main()
{
    std::ios_base::sync_with_stdio(0);
    sf::RenderWindow window(sf::VideoMode(1270, 768), "A*");
    window.setVerticalSyncEnabled(true);
    bool canPlace{ true };
    bool modeBuildConnection{ false };
    //astar::Graph::getInstance().setDrawDistance(true);
    
    while (window.isOpen())
    {
        if (!window.hasFocus()) 
        {
            continue;
        }
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Vector2f mousePos(sf::Mouse::getPosition(window));

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !modeBuildConnection)
        {
            if (canPlace)
            {
                astar::Graph::getInstance().addNode(mousePos);
                canPlace = false;
            }
        }
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && modeBuildConnection) {
            if(canPlace)
            {
                astar::Graph::getInstance().makeConnection(mousePos);
                canPlace = false;
            }
            
        }
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            astar::Graph::getInstance().checkAndDelete(mousePos);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt)) {
            if (canPlace)
            {
                astar::Graph::getInstance().setCollision(mousePos);
                canPlace = false;
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        {
            if (canPlace) {
                modeBuildConnection = !modeBuildConnection;
                canPlace = false;
            }
        }
        else
        {
            canPlace = true;
        }


        window.clear();
        astar::Graph::getInstance().draw(window, mousePos, modeBuildConnection);

        window.display();
    }
}
