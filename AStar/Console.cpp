#include "Console.hpp"
#include <iostream>
#include <algorithm>


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
		callbacks_.emplace_back("reset", [](std::optional<std::vector<std::string>> args){ astar::Graph::getInstance().resetNodes(); }, false);
		callbacks_.emplace_back("conn", [](std::optional<std::vector<std::string>> args){ astar::Graph::getInstance().toggleConnectionMode(); }, false);
		callbacks_.emplace_back("del", [](std::optional<std::vector<std::string>> args){
			if (std::all_of(args->at(0).begin(), args->at(0).end(), [](const char c) {return std::isdigit(c); }))
			{
				std::cout << "executing command: del " << std::stoi(args->at(0)) << '\n';
				astar::Graph::getInstance().deleteNode((std::stoi(args->at(0))));
			}
		}, true);
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

	void Console::executeCommand(std::string& command)
	{
		if (command[0] == ' ')
		{
			auto found_l = std::find_if(command.begin(), command.end(), [](const char c) { return c != ' '; });

			if (found_l != command.end())
			{
				command.erase(command.begin(), found_l);
			}
		}

		if (command.back() == ' ')
		{
			for (int i = command.size() - 1; i >= 0; i--)
			{
				if (command[i] != ' ')
				{
					std::cout << "command[" << i << "] is: " << command[i] << " deleting\n";
					command.erase(command.begin() + i + 1, command.end());
					break;
				}
				else
				{
					std::cout << "command[" << i << "] is: " << command[i] << " not deleting\n";
				}
			}
		}

		std::vector<std::string> split;
		split.reserve(2);
		size_t pos = 0;
		std::string token;

		if (std::count(command.begin(), command.end(), ' ') > 0)
		{
			while ((pos = command.find(' ')) != std::string::npos)
			{
				split.push_back(command.substr(0, pos));
				command.erase(0, pos + 1);
				split.push_back(command);
			}
		}
		else
		{
			split.push_back(command);
		}


		if (const auto& pos = std::find_if(callbacks_.begin(), callbacks_.end(), [command = split[0]](const Command& cmd) { return std::get<0>(cmd) == command; }); pos != callbacks_.end())
		{
			if (!std::get<2>(*pos))
			{
				history_.push_back(split[0]);
				std::get<1>(*pos)(std::nullopt);
			}
			else
			{
				if (split.size() < 2)
				{
					history_.push_back("&&R" + split[0] + " - command expects arguments!");

					if (history_.size() >= 40)
					{
						history_.erase(history_.begin());
					}
				}
				else
				{
					history_.push_back(split[0] + ' ' + split[1]);
					std::get<1>(*pos)(std::vector{ split[1] });
				}
			}
		}
		else if(!currentText_.empty())
		{
			history_.push_back("&&R" + split[0] + " - unknown command!");
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
			rect.setFillColor(sf::Color(0, 0, 0, 175));
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
