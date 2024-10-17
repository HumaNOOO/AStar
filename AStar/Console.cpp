#include "Console.hpp"
#include <iostream>


namespace astar
{
	Console& Console::getInstance()
	{
		static Console console;
		return console;
	}

	Console::Console() : consoleOpen_{ false }, cursorPos_{ 1 }
	{
		std::cout << "Console::Console()\n";
		callbacks_.emplace_back("reset", [](std::optional<std::vector<std::string>> args){ astar::Graph::getInstance().resetNodes(); });
		callbacks_.emplace_back("conn", [](std::optional<std::vector<std::string>> args){ astar::Graph::getInstance().toggleConnectionMode(); });
		font_.loadFromFile("mono.ttf");
		text_.setFont(font_);
		text_.setCharacterSize(16);
		history_.reserve(20);
		carriage_.setFillColor(sf::Color::White);
		carriage_.setSize({ 1, 16 });
	}

	void Console::handleInput(const sf::Keyboard::Key key)
	{
		if (key == sf::Keyboard::Tilde)
		{
			consoleOpen_ = !consoleOpen_;
		}
		else if (key == 13 && !currentText_.empty())
		{
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
			carriage_.move(carriageOffset_, 0);
			++cursorPos_;
		}
		else if(c == 8)
		{
			if (!currentText_.empty() && cursorPos_ > 1)
			{
				currentText_.pop_back();
				carriage_.move(-carriageOffset_, 0);
				--cursorPos_;
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

			if (history_.size() >= 40)
			{
				history_.erase(history_.begin());
			}
		}

		currentText_.clear();
		carriage_.setPosition(4, carriage_.getPosition().y);
	}

	void Console::resetCarriage(const sf::Vector2f& carriagePos)
	{
		carriage_.setPosition(carriagePos);
	}

	void Console::draw(sf::RenderTarget& rt)
	{
		if (consoleOpen_)
		{
			const auto& size = rt.getSize();

			sf::RectangleShape rect(sf::Vector2f{ size });
			rect.setFillColor(sf::Color(0, 0, 0, 30));
			text_.setString(currentText_);
			text_.setFillColor(sf::Color::White);
			text_.setPosition(4, size.y - 22);
			rt.draw(rect);
			rt.draw(text_);

			float textPos{ 40.f };
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

				text_.setPosition(4, size.y - (textPos += 18));
				text_.setString(cpy);
				rt.draw(text_);
			}

			rt.draw(carriage_);
		}
	}

	void Console::moveCarriage(const bool left)
	{
		if (!currentText_.empty())
		{
			if (left && cursorPos_ > 1)
			{
				--cursorPos_;
				carriage_.move(-carriageOffset_, 0);
				std::cout << "carriage x: " << carriage_.getPosition().x << ", pos: " << cursorPos_ << '\n';
			}
			else if(!left && cursorPos_ <= currentText_.size())
			{
				++cursorPos_;
				carriage_.move(carriageOffset_, 0);
				std::cout << "carriage x: " << carriage_.getPosition().x << ", pos: " << cursorPos_ << '\n';
			}
		}
	}
}
