#include <iostream>

#include "Console.hpp"


int main()
{
	std::ios_base::sync_with_stdio(0);
	sf::ContextSettings cs;
	cs.antialiasingLevel = 16;
	constexpr unsigned windowWidth{ 1270 };
	constexpr unsigned windowHeight{ 768 };
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "A*", sf::Style::Default, cs);
	astar::Graph::get().setRenderTarget(&window);
	sf::View currentView{ sf::FloatRect(0, 0, windowWidth, windowHeight) };
	astar::Console::get().resetCarriage({ 4, windowHeight - 20 });
	window.setVerticalSyncEnabled(true);
	bool movingNode{ false };
	bool movingView{ false };
	astar::Graph::get().setDrawIds(true);
	sf::Clock clk;
	bool rapidConnect{ false };
	float rapidConnectDelay{ 0.01f };
	float zoom{ 1.f };

	//astar::Graph::get().toggleDrawScore();
	sf::Vector2f oldMousePos;

	while (window.isOpen())
	{
		sf::Event event;
		sf::Vector2f mousePos{ sf::Mouse::getPosition(window) };
		sf::Vector2f mousePosTranslated{ window.mapPixelToCoords(sf::Mouse::getPosition(window),currentView) };

		while (window.pollEvent(event))
		{
			if (movingNode)
			{
				if (event.type == sf::Event::MouseButtonReleased)
				{
					movingNode = false;
					astar::Graph::get().clearSavedNode();
				}
				else 
				{
					astar::Graph::get().moveNode(mousePosTranslated);
					break;
				}
			}

			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				currentView = sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height)));
				zoom = 1.f;
				window.setView(currentView);
				astar::Console::get().resetCarriage({ 4, static_cast<float>(event.size.height) });
				break;
			case sf::Event::TextEntered:
				if (astar::Console::get().isOpen())
				{
#ifdef _DEBUG
					std::cout << "key pressed: " << event.key.code << '\n';
#endif
					astar::Console::get().addChar(event.text.unicode);
					astar::Console::get().handleInput(event.key.code);
				}
			break;
			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::Tilde:
					astar::Console::get().toggle();
					break;
				case sf::Keyboard::Escape:
					window.close();
					break;
				case sf::Keyboard::Q:
					astar::Graph::get().selectNodes(mousePosTranslated);
					break;
				case sf::Keyboard::O:
					astar::Graph::get().increaseOffset(1);
					break;
				case sf::Keyboard::C:
					astar::Graph::get().clearSavedNode();
					break;
				case sf::Keyboard::P:
					astar::Graph::get().increaseOffset(-1);
					break;
				case sf::Keyboard::V:
					astar::Graph::get().toggleRapidConnect();
					rapidConnect = !rapidConnect;
					break;
				case sf::Keyboard::B:
					astar::Graph::get().toggleConnectionMode();
					break;
				case sf::Keyboard::LAlt:
					astar::Graph::get().setCollision(mousePosTranslated);
					break;
				case sf::Keyboard::Left:
					astar::Console::get().moveCarriage(true);
					break;
				case sf::Keyboard::Right:
					astar::Console::get().moveCarriage(false);
					break;
				}
				break;
			case sf::Event::MouseButtonPressed:
				switch (event.mouseButton.button)
				{
				case sf::Mouse::Left:
					if (!astar::Graph::get().isBuildConnectionMode() && !sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
					{
						if (astar::Graph::get().checkMouseOnSomething(mousePosTranslated))
						{
							movingNode = true;
						}
						else
						{
							astar::Graph::get().addNode(mousePosTranslated);
						}
					}
					else
					{
						astar::Graph::get().makeConnection(mousePosTranslated);
					}
					break;
				case sf::Mouse::Right:
					if (astar::Graph::get().isBuildConnectionMode())
					{
						astar::Graph::get().setCollision(mousePosTranslated);
					}
					else
					{
						astar::Graph::get().checkAndDelete(mousePosTranslated);
					}
					break;
				}
				break;
			case sf::Event::MouseWheelScrolled:
				if (event.mouseWheelScroll.delta == 1 && zoom > 0.1f)
				{
					zoom *= 0.9f;
					currentView.zoom(0.9f);
				}
				else if(event.mouseWheelScroll.delta == -1 && zoom < 10.f)
				{
					zoom *= 1.1f;
					currentView.zoom(1.1f);
				}
			}
		}

		if (rapidConnect &&
			rapidConnectDelay <= 0.f &&
			sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
			sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
			astar::Graph::get().isBuildConnectionMode())
		{
			astar::Graph::get().makeConnection(mousePosTranslated);
			rapidConnectDelay = 0.01f;
		}

		rapidConnectDelay -= clk.restart().asSeconds();

		movingView = false;
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		{
			currentView.move((oldMousePos - mousePos) * zoom);
			movingView = true;
		}

		oldMousePos = mousePos;
		window.setView(currentView);
		window.clear();
		astar::Graph::get().draw(mousePosTranslated);
		window.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y))));
		astar::Console::get().draw(window);
		astar::Graph::get().drawStats();
		window.display();
	}
}
