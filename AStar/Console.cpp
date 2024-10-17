#include "Console.hpp"
#include <iostream>


namespace astar
{
	Console& Console::getInstance()
	{
		static Console console;
		return console;
	}

	Console::Console() : consoleOpen_{ false }, cursorPos_{ 0 }
	{
		std::cout << "Console::Console()\n";
		callbacks_.emplace_back("reset", [](std::optional<std::vector<std::string>> args){ astar::Graph::getInstance().resetNodes(); });
		callbacks_.emplace_back("conn", [](std::optional<std::vector<std::string>> args){ astar::Graph::getInstance().toggleConnectionMode(); });
		font_.loadFromFile("mono.ttf");
		text_.setFont(font_);
		text_.setCharacterSize(16);
		history_.reserve(20);
	}

	void Console::handleInput(const sf::Keyboard::Key key)
	{
		if (key == sf::Keyboard::Tilde)
		{
			consoleOpen_ = !consoleOpen_;
		}
		else if (key == 13 && !currentText_.empty())
		{
			std::cout << "executing command: " << currentText_ << '\n';
			executeCommand(currentText_);
		}

		std::cout << "current text size: " << currentText_.size() << '\n';
		std::cout << "keycode: " << key << '\n';
	}

	void Console::toggle()
	{
		consoleOpen_ = !consoleOpen_;
	}

	void Console::addChar(const char c)
	{
		if (c != 8 && c != '`' && c != 13)
		{
			currentText_ += c;
		}
		else if(c == 8)
		{
			if (!currentText_.empty())
			{
				currentText_.pop_back();
			}
		}
	}

	bool Console::isOpen() const
	{
		return consoleOpen_;
	}

	void Console::executeCommand(const std::string& command, std::optional<std::vector<std::string>> args)
	{
		if (const auto& pos = std::find_if(callbacks_.begin(), callbacks_.end(), [&command](const Command& cmd) { return std::get<0>(cmd) == command; }); pos != callbacks_.end())
		{
			history_.push_back(command);
			std::get<1>(*pos)(args);
		}
		else if(!currentText_.empty())
		{
			history_.push_back("&&R" + command + " - unknown command!");
			std::cout << "history size: " << history_.size() << '\n';
			std::cout << "current text size: " << currentText_.size() << '\n';

			if (history_.size() >= 20)
			{
				history_.erase(history_.begin());
			}
		}

		currentText_.clear();
	}

	void Console::draw(sf::RenderTarget& rt)
	{
		if (consoleOpen_)
		{
			const auto& size = rt.getSize();

			sf::RectangleShape rect(sf::Vector2f{ size });
			rect.setFillColor(sf::Color(0, 0, 0, 70));
			text_.setString(currentText_);
			text_.setFillColor(sf::Color::White);
			text_.setPosition(5, size.y - 20);
			rt.draw(rect);
			rt.draw(text_);

			float textPos{ 30.f };
			for (std::vector<std::string>::reverse_iterator rIter{ history_.rbegin() }; rIter != history_.rend(); ++rIter)
			{
				std::string cpy;
				if (rIter->find("&&R") != std::string::npos)
				{
					text_.setFillColor(sf::Color::Red);
					cpy = rIter->substr(3);
				}
				else
				{
					text_.setFillColor(sf::Color::White);
					cpy = *rIter;
				}

				text_.setPosition(5, size.y - (textPos += 18));
				text_.setString(cpy);
				rt.draw(text_);
			}
		}
	}
}
