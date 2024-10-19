#include "Console.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <format>


namespace astar
{
	Console& Console::getInstance()
	{
		static Console console;
		return console;
	}

	Console::Console() : consoleOpen_{ false }, carriagePos_{ 1 }
	{
		callbacks_.emplace_back("reset", [](std::optional<std::vector<std::string>> args){ astar::Graph::getInstance().resetNodes(); }, false);
		callbacks_.emplace_back("clear", [this](std::optional<std::vector<std::string>> args) { history_.clear(); }, false);
		callbacks_.emplace_back("distance", [](std::optional<std::vector<std::string>> args) { astar::Graph::getInstance().toggleDrawDistance(); }, false);
		callbacks_.emplace_back("conn", [](std::optional<std::vector<std::string>> args){ astar::Graph::getInstance().toggleConnectionMode(); }, false);
		callbacks_.emplace_back("del", [](std::optional<std::vector<std::string>> args)
		{
			try
			{
				astar::Graph::getInstance().deleteNode((std::stoi(args->at(0))));
			}
			catch (std::exception& e)
			{
#ifdef _DEBUG
				std::cout << "argument is not a number: " << e.what() << '\n';
#endif
			}
		}, true);
		callbacks_.emplace_back("print", [this](std::optional<std::vector<std::string>> args) 
		{ 
				const auto& connections = astar::Graph::getInstance().getConnectionsCRef();

				if (connections.empty())
				{
					history_.emplace_back("&&Gno connections to print");
					return;
				}

				std::stringstream ss;
				ss << "&&G" << connections.size() << " node connections:\n";
				std::ranges::for_each(connections, [&ss](const std::pair<int, int>& connection)
				{
						ss << connection.first << "<->" << connection.second << '\n';
				});
				history_.emplace_back(ss.str());
		}, false);
		callbacks_.emplace_back("link", [this](std::optional<std::vector<std::string>> args)
		{ 
				const std::vector<std::string>& argsRef = *args;
				std::vector<long> ids;
				ids.reserve(argsRef.size());

				if (argsRef.size() < 2)
				{
					history_.emplace_back("&&Rincorrect number of arguments, need 2 or more!");
					return;
				}

				std::stringstream ss;
				ss << "&&R";
				for (const auto& str : argsRef)
				{
					try
					{
						ids.push_back(std::stoi(str));
					}
					catch (const std::exception& e)
					{
						ss << "can't convert '" << str << "' into a number: " << e.what() << '\n';
					}
				}
					
				if (ss.str().size() > 3)
				{
					history_.emplace_back(ss.str());
				}

				ss.str("");
				ss << "&&G";
				for (int i = 1; i < ids.size(); i++)
				{
					if (astar::Graph::getInstance().addIdConnection({ ids[0], ids[i] }))
					{
						ss << "adding connection " << ids[0] << "<->" << ids[i] << '\n';
					}
				}
				history_.emplace_back(ss.str());
		}, true);
		callbacks_.emplace_back("path", [](std::optional<std::vector<std::string>> args)
		{
				return;
		}, true);
		callbacks_.emplace_back("load", [this](std::optional<std::vector<std::string>> args)
			{ 
				std::fstream file;
				file.open(args->at(0), std::ios::in);

				if (!file.is_open())
				{
					history_.emplace_back("&&Rcan't open file '" + args->at(0) + "'!\n");
					return;
				}

				std::string line;
				std::vector<std::tuple<float, float, int>> splitNodes;
				std::vector<std::string> splitPrepare;
				std::size_t pos;

				while (std::getline(file, line) && line != "\n")
				{
					splitPrepare.push_back(line);
					std::cout << line << '\n';
				}

				for (auto& str : splitPrepare)
				{
					if (std::ranges::count(str, ';') != 2)
					{
						history_.emplace_back("&&Rill formed data '" + str + "'!\n");
						file.close();
						return;
					}

					int count{ 0 };
					splitNodes.push_back({});
					while ((pos = line.find(';')) != std::string::npos && count < 3)
					{
						auto& [x, y, id] = splitNodes.back();

						try
						{
							if (count == 0)
							{
								x = std::stof(str.substr(0, pos));
							}
							else if (count == 1)
							{
								y = std::stof(str.substr(0, pos));
							}
							else
							{
								id = std::stoi(str.substr(0, pos));
							}
							str.erase(0, pos + 1);
							++count;
						}
						catch (const std::exception& e)
						{
							history_.emplace_back("&&Rbad data - " + std::string(e.what()) + '\n');
							file.close();
							return;
						}
					}
				}

				astar::Graph::getInstance().resetNodes();

				for (const auto& [x, y, id] : splitNodes)
				{
#ifdef _DEBUG
					std::cout << std::format("{}, {}, {}\n", x, y, id);
#endif
					astar::Graph::getInstance().addNode({ x,y }, id);
				}

				astar::Graph::getInstance().resetIndex();

				file.close();
			}, true);
		font_.loadFromFile("mono.ttf");
		text_.setFont(font_);
		text_.setCharacterSize(16);
		history_.reserve(100);
		carriage_.setFillColor(sf::Color::White);
		carriage_.setSize({ 1, 16 });
	}

	void Console::handleInput(const sf::Keyboard::Key key)
	{
		if (key == sf::Keyboard::Tilde && key != '6')
		{
			consoleOpen_ = !consoleOpen_;
		}
		else if (key == 13 && !currentText_.empty())
		{
			executeCommand(currentText_);
		}
		else if (key == sf::Keyboard::Left)
		{
			moveCarriage(true);
		}
		else if (key == sf::Keyboard::Right)
		{
			moveCarriage(false);
		}

#ifdef _DEBUG
		std::cout << "current text size: " << currentText_.size() << '\n';
		std::cout << "keycode: " << key << '\n';
#endif
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
			moveCarriage(false);
		}
		else if(c == 8)
		{
			if (!currentText_.empty() && carriagePos_ > 1)
			{
				moveCarriage(true);
				currentText_.pop_back();
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
			auto found_l = std::ranges::find_if(command, [](const char c) { return c != ' '; });

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
					command.erase(command.begin() + i + 1, command.end());
					break;
				}
			}
		}

		std::vector<std::string> split;
		split.reserve(2);
		size_t pos = 0;
		
		if (std::ranges::count(command, ' ') > 0)
		{
			while ((pos = command.find(' ')) != std::string::npos)
			{
				split.push_back(command.substr(0, pos));
				command.erase(0, pos + 1);
			}
			split.push_back(command);
		}
		else
		{
			split.push_back(command);
		}


		if (const auto& pos = std::ranges::find_if(callbacks_, [&command = split[0]](const Command& cmd) { return std::get<0>(cmd) == command; }); pos != callbacks_.end())
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
					std::stringstream ss;
					ss << split[0] << ' ';
					for (int i = 1; i < split.size(); i++)
					{
						ss << split[i] << ' ';
					}

					history_.push_back(ss.str());
					std::get<1>(*pos)(std::vector(split.begin() + 1, split.end()));
				}
			}
		}
		else if(!currentText_.empty())
		{
			history_.push_back("&&R" + split[0] + " - unknown command!");
#ifdef _DEBUG
			std::cout << "history size: " << history_.size() << '\n';
			std::cout << "current text size: " << currentText_.size() << '\n';
#endif

			if (history_.size() >= 40)
			{
				history_.erase(history_.begin());
			}
		}

		currentText_.clear();
		carriagePos_ = 1;
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
				else if (rIter->find("&&G") != std::string::npos)
				{
					text_.setFillColor(sf::Color::Green);
					cpy = rIter->substr(3);
				}
				else
				{
					text_.setFillColor(sf::Color::White);
					cpy = *rIter;
				}

				const int newlines = std::count(cpy.begin(), cpy.end(), '\n');

				if (newlines)
				{
					text_.setPosition(4, size.y - (textPos += (22 * (newlines))));
				}
				else
				{
					text_.setPosition(4, size.y - (textPos += 18));
				}

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
			if (left && carriagePos_ > 1)
			{
				--carriagePos_;
				carriage_.move(-carriageOffset_, 0);
#ifdef _DEBUG
				std::cout << "carriage x: " << carriage_.getPosition().x << ", pos: " << carriagePos_ << '\n';
#endif
			}
			else if(!left && carriagePos_ <= currentText_.size())
			{
				++carriagePos_;
				carriage_.move(carriageOffset_, 0);
#ifdef _DEBUG
				std::cout << "carriage x: " << carriage_.getPosition().x << ", pos: " << carriagePos_ << '\n';
#endif
			}
		}
	}
}
