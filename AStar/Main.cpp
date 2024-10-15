//test czy git działa

#include <iostream>

#include "Graph.hpp"


int main()
{
	std::ios_base::sync_with_stdio(0);
	sf::ContextSettings cs;
	cs.antialiasingLevel = 16;
	sf::RenderWindow window(sf::VideoMode(1270, 768), "A*", sf::Style::Default, cs);
	window.setVerticalSyncEnabled(true);
	bool modeBuildConnection{ false };
	bool movingNode{ false };
	astar::Graph::getInstance().setDrawDistance(false);
	astar::Graph::getInstance().setDrawIds(true);
	while (window.isOpen())
	{
		if (!window.hasFocus())
		{
			continue;
		}

		sf::Event event;
		sf::Vector2f mousePos(sf::Mouse::getPosition(window));

		while (window.pollEvent(event))
		{
			if (movingNode) {
				if(event.type==10)
				{
					movingNode = false;
					astar::Graph::getInstance().clearSavedNode();
				}
				else {
					astar::Graph::getInstance().moveNode(mousePos);
					break;
				}
			}
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::R:
					astar::Graph::getInstance().resetNodes();
					break;
				case sf::Keyboard::Escape:
					window.close();
					break;
				case sf::Keyboard::LShift:
					modeBuildConnection = !modeBuildConnection;
					astar::Graph::getInstance().clearSavedNode();
					break;
				case sf::Keyboard::Q:
					astar::Graph::getInstance().selectNodes(mousePos);
					break;
				case sf::Keyboard::O:
					astar::Graph::getInstance().increaseOffset(-1);
					break;
				case sf::Keyboard::P:
					astar::Graph::getInstance().increaseOffset(1);
					break;
				case sf::Keyboard::LAlt:
					astar::Graph::getInstance().setCollision(mousePos);
					break;
				}
			case sf::Event::MouseButtonPressed:
				switch (event.mouseButton.button)
				{
				case sf::Mouse::Left:
					if (!modeBuildConnection)
					{
						auto checkmouseup = astar::Graph::getInstance().checkMouseOnSomething(mousePos);
						if(checkmouseup)
						{
							movingNode = true;
						}
						else
						{
							astar::Graph::getInstance().addNode(mousePos);
						}
						
					}
					else
					{
						astar::Graph::getInstance().makeConnection(mousePos);
					}
					break;
				case sf::Mouse::Right:
					astar::Graph::getInstance().checkAndDelete(mousePos);
					break;
				}
			}
		}

		window.clear();
		astar::Graph::getInstance().draw(window, mousePos, modeBuildConnection);
		window.display();
	}
}
