#include <iostream>

#include "Console.hpp"


int main()
{
	std::ios_base::sync_with_stdio(0);
	sf::ContextSettings cs;
	cs.antialiasingLevel = 16;
	sf::RenderWindow window(sf::VideoMode(1270, 768), "A*", sf::Style::Default, cs);
	window.setVerticalSyncEnabled(true);
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
				window.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));
				break;
			case sf::Event::TextEntered:
				if (astar::Console::getInstance().isOpen())
				{
					astar::Console::getInstance().addChar(event.text.unicode);
					astar::Console::getInstance().handleInput(event.key.code);
				}
			break;
			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::Tilde:
					astar::Console::getInstance().toggle();
					break;
				case sf::Keyboard::Escape:
					window.close();
					break;
				case sf::Keyboard::LShift:
					astar::Graph::getInstance().clearSavedNode();
					break;
				case sf::Keyboard::Q:
					astar::Graph::getInstance().selectNodes(mousePos);
					break;
				case sf::Keyboard::O:
					astar::Graph::getInstance().increaseOffset(1);
					break;
				case sf::Keyboard::P:
					astar::Graph::getInstance().increaseOffset(-1);
					break;
				case sf::Keyboard::LAlt:
					astar::Graph::getInstance().setCollision(mousePos);
					break;
				}
				break;
			case sf::Event::MouseButtonPressed:
				switch (event.mouseButton.button)
				{
				case sf::Mouse::Left:
					if (!astar::Graph::getInstance().isBuildConnectionMode())
					{
						if (astar::Graph::getInstance().checkMouseOnSomething(mousePos))
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
		astar::Graph::getInstance().draw(window, mousePos);
		if (astar::Console::getInstance().isOpen())
		{
			astar::Console::getInstance().draw(window);
		}
		window.display();
	}
}
