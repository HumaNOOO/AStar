#include "Console.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <format>
#include <filesystem>


namespace
{
	template<typename Type>
	void splitString(std::vector<Type>& vec, std::string str, const char delim)
	{
		if constexpr (std::is_same<Type, std::string>::value)
		{
			size_t pos;
			while ((pos = str.find(delim)) != std::string::npos)
			{
				vec.emplace_back(str.substr(0, pos));
				str.erase(0, pos + 1);
			}
			vec.emplace_back(str);
		}
		else if constexpr (std::is_same<Type, float>::value)
		{
			std::vector<Type> temp;

			try
			{
				size_t pos;
				while ((pos = str.find(delim)) != std::string::npos)
				{
					temp.emplace_back(std::stof(str.substr(0, pos)));
					str.erase(0, pos + 1);
				}
				temp.emplace_back(std::stof(str));
			}
			catch (...)
			{
				return;
			}

			vec = std::move(temp);
		}
	}
}

namespace astar
{
	Console& Console::get()
	{
		static Console console;
		return console;
	}

	Console::Console() : consoleOpen_{ false }, carriagePos_{ 1 }
	{
		callbacks_.emplace_back("reset", [](const std::vector<std::string>& args) { Graph::get().resetNodes(); }, false);
		callbacks_.emplace_back("clear", [this](const std::vector<std::string>& args) { history_.clear(); }, false);
		callbacks_.emplace_back("distance", [](const std::vector<std::string>& args) { Graph::get().toggleDrawDistance(); }, false);
		callbacks_.emplace_back("conn", [](const std::vector<std::string>& args) { Graph::get().toggleConnectionMode(); }, false);
		callbacks_.emplace_back("del", [](const std::vector<std::string>& args)
			{
				try
				{
					Graph::get().deleteNode((std::stoi(args[0])));
				}
				catch (const std::exception& e)
				{
#ifdef _DEBUG
					std::cout << "argument is not a number: " << e.what() << '\n';
#endif
				}
			}, true);
		callbacks_.emplace_back("print", [this](const std::vector<std::string>& args)
			{
				const auto& connections = Graph::get().connectionsCRef();
				const auto& connectionsCached = Graph::get().connectionsCachedCRef();

				if (connections.empty())
				{
					history_.emplace_back("&&Gno connections to print");
					return;
				}

				std::stringstream ss;
				ss << "&&G" << connectionsCached.size() << " cached connections\n" << connections.size() << " node connections:\n";
				std::ranges::for_each(connections, [&ss](const std::pair<int, int>& connection)
					{
						ss << connection.first << "<->" << connection.second << '\n';
					});
				history_.emplace_back(ss.str());
			}, false);
		callbacks_.emplace_back("link", [this](const std::vector<std::string>& args)
			{
				std::vector<int> ids;
				ids.reserve(args.size());

				if (args.size() < 2)
				{
					history_.emplace_back("&&Rincorrect number of arguments, need 2 or more!");
					return;
				}

				std::stringstream ss;
				ss << "&&R";
				for (const auto& str : args)
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
					if (Graph::get().addIdConnection({ ids[0], ids[i] }))
					{
						ss << "adding connection " << ids[0] << "<->" << ids[i] << '\n';
					}
				}
				history_.emplace_back(ss.str());
			}, true);
		callbacks_.emplace_back("path", [this](const std::vector<std::string>& args)
			{
				std::vector<int> ids;
				ids.reserve(args.size());

				if (args.size() < 2)
				{
					history_.emplace_back("&&Rincorrect number of arguments, need 2 or more!");
					return;
				}

				std::stringstream ss;
				ss << "&&R";
				for (const auto& str : args)
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
				for (int i = 0; i < ids.size() - 1; i++)
				{
					if (Graph::get().addIdConnection({ ids[i], ids[i+1] }))
					{
						ss << "adding connection " << ids[i] << "<->" << ids[i+1] << '\n';
					}
				}
				if (ss.str().size() == 3)
				{
					history_.emplace_back("&&Rcommand failed");
					return;
				}
				history_.emplace_back(ss.str());
			}, true);
		callbacks_.emplace_back("set", [this](const std::vector<std::string>& args)
			{
				if (args.size() < 2)
				{
					history_.emplace_back("&&Rincorrect number of arguments, need 2!");
					return;
				}

				if (args[0] == "start")
				{
					try
					{
						if (Graph::get().setStart(std::stoi(args[1])))
						{
							history_.emplace_back("&&Gstart node id " + args[1]);
						}
						else
						{
							history_.emplace_back("&&Rnode with id " + args[1] + " doesn't exist!");
						}
					}
					catch (const std::exception& e)
					{
						history_.emplace_back("&&Rcan't convert '" + args[1] + "' to a number: " + e.what());
					}
				}
				else if (args[0] == "end")
				{
					try
					{
						if (Graph::get().setEnd(std::stoi(args[1])))
						{
							history_.emplace_back("&&Gend node id " + args[1]);
						}
						else
						{
							history_.emplace_back("&&Rnode with id " + args[1] + " doesn't exist!");
						}
					}
					catch (const std::exception& e)
					{
						history_.emplace_back("&&Rcan't convert '" + args[1] + "' to a number: " + e.what());
					}
				}
				else
				{
					history_.emplace_back("&&Runknown parameter '" + args[0] + "'!");
				}
			}, true);
		callbacks_.emplace_back("load", [this](const std::vector<std::string>& args)
			{
				std::fstream file;
				file.open(args[0], std::ios::in);

				if (!file.is_open())
				{
					history_.emplace_back("&&Rcan't open file '" + args[0] + "'!\n");
					return;
				}

				std::string line;
				std::vector<std::tuple<float, float, int, bool>> splitNodes;
				std::vector<std::string> stringsToSplit;

				while (std::getline(file, line) && line != "\n")
				{
					stringsToSplit.push_back(line);
#ifdef _DEBUG
					std::cout << line << '\n';
#endif
				}

				std::stringstream ss;
				ss << "&&G";

				Graph::get().resetNodes();
				std::vector<std::vector<float>> connectionsVec;

				int count{};
				for (auto& str : stringsToSplit)
				{
					++count;
					if (std::ranges::count(str, ',') != 4)
					{
						history_.emplace_back("&&Rill formed data '" + str + "' - skipping\n");
						continue;
					}

					std::vector<std::string> split;
					splitString(split, str, ',');

					if (split.size() != 5)
					{
						history_.emplace_back(std::format("&&Rnumber of fields in '{}' at line number {} is wrong, expected 5 but got {} - skipping\n", str, count, split.size()));
						continue;
					}

					std::vector<float> connections;
					splitString(connections, split.back(), ':');

					if (connections.empty())
					{
						history_.emplace_back(std::format("&&Rbad argument in '{}' at line number {}  - skipping\n", str, count));
						continue;
					}
					else if (connections.size() < 2)
					{
						history_.emplace_back("&&Rneed 2 or more ids for connections - skipping\n");
						continue;
					}

					split.pop_back();
					connectionsVec.push_back(std::move(connections));

					try
					{
						splitNodes.push_back({ std::stof(split[0]), std::stof(split[1]), std::stof(split[2]), std::stoi(split[3]) });
					}
					catch (const std::exception& e)
					{
						history_.emplace_back("&&Rbad data - " + std::string(e.what()) + '\n');
						return;
					}
				}

				for (const auto& [x, y, id, isCollision] : splitNodes)
				{
					if (Graph::get().addNode({ x,y }, id, isCollision))
					{
						ss << std::format("adding node at ({},{}) with id {} and collision {}\n", x, y, id, isCollision ? "on" : "off");
					}
					else
					{
						ss << std::format("node with id {} already exists - skipping\n", id);
					}
				}

				for (const auto& connection : connectionsVec)
				{
					for (int i = 1; i < connection.size(); i++)
					{
						if (Graph::get().addIdConnection({ connection[0], connection[i] }))
						{
							ss << "adding connection " << connection[0] << "<->" << connection[i] << '\n';
						}
						else
						{
							ss << std::format("connection {}<->{} already exists - skipping\n", connection[0], connection[i]);
						}
					}
				}

				Graph::get().resetIndex();

				history_.emplace_back(ss.str());
			}, true);
		callbacks_.emplace_back("save", [this](const std::vector<std::string>& args)
			{
				std::fstream file;
				file.open(args[0], std::ios::out);
				const std::vector<Node>& nodesRef = Graph::get().nodesCRef();

				if (!file.is_open())
				{
					history_.emplace_back("&&Rcan't open file '" + args[0] + "'!\n");
					return;
				}
				else if (nodesRef.empty())
				{
					file.close();
					history_.emplace_back("&&Rno nodes to save!\n");
					std::filesystem::remove(std::filesystem::path(args[0]));
					return;
				}
				
				std::stringstream ss;
				ss << "&&G";

				for (const Node& node : nodesRef)
				{
					std::string connections;

					connections += (std::to_string(node.id()) + ':');

					for (const Node* conn : node.connections_)
					{
						connections += (std::to_string(conn->id()) + ':');
					}
					connections.erase(connections.end() - 1);

					file << std::format("{},{},{},{},{}\n", node.pos().x, node.pos().y, node.id(), static_cast<int>(node.isCollision()), connections);

					ss << std::format("saving {},{},{},{},{}\n", node.pos().x, node.pos().y, node.id(), static_cast<int>(node.isCollision()), connections);
				}

				ss << "graph saved to file '" << args[0] << "'!\n";
				history_.emplace_back(ss.str());
			}, true);
		callbacks_.emplace_back("exec", [this](const std::vector<std::string>& args)
			{
				if (args[0] == "astar")
				{
					auto result = Graph::get().executeAStar();

					//return;

					if (result)
					{
						history_.emplace_back("&&Gpath found, execution time: " + *result + "s");
					}
					else
					{
						history_.emplace_back("&&Gno path found");
					}
				}
				else
				{
					history_.emplace_back("&&Runknown parameter '" + args[0] + "'!");
				}
			}, true);
		callbacks_.emplace_back("save", [this](const std::vector<std::string>& args)
			{
				std::fstream file;
				file.open(args[0], std::ios::out);
				const std::vector<Node>& nodesRef = Graph::get().nodesCRef();

				if (!file.is_open())
				{
					history_.emplace_back("&&Rcan't open file '" + args[0] + "'!\n");
					return;
				}
				else if (nodesRef.empty())
				{
					file.close();
					history_.emplace_back("&&Rno nodes to save!\n");
					std::filesystem::remove(std::filesystem::path(args[0]));
					return;
				}

				std::stringstream ss;
				ss << "&&G";

				for (const Node& node : nodesRef)
				{
					std::string connections;

					connections += (std::to_string(node.id()) + ':');

					for (const Node* conn : node.connections_)
					{
						connections += (std::to_string(conn->id()) + ':');
					}
					connections.erase(connections.end() - 1);

					file << std::format("{},{},{},{},{}\n", node.pos().x, node.pos().y, node.id(), static_cast<int>(node.isCollision()), connections);

					ss << std::format("saving {},{},{},{},{}\n", node.pos().x, node.pos().y, node.id(), static_cast<int>(node.isCollision()), connections);
				}

				ss << "graph saved to file '" << args[0] << "'!\n";
				history_.emplace_back(ss.str());
			}, true);
		callbacks_.emplace_back("generate", [this](const std::vector<std::string>& args)
			{
				if (args.size() != 3)
				{
					history_.emplace_back("&&Rincorrect number of arguments, need 3!");
					return;
				}

				std::vector<float> nodesConnectionsCount;

				for (const std::string& arg : args)
				{
					try
					{
						for (const std::string& arg : args)
						{
							nodesConnectionsCount.push_back(std::stof(arg));
						}
					}
					catch (const std::exception& e)
					{
						history_.emplace_back("&&Rcan't convert '" + arg + "' to a number: " + e.what());
						return;
					}
				}

				Graph::get().generateRandomGraph(nodesConnectionsCount[0], nodesConnectionsCount[1], nodesConnectionsCount[2]);

				history_.emplace_back("&&Ggenerated graph");
			}, true);
		callbacks_.emplace_back("generatetimeout", [this](const std::vector<std::string>& args)
			{
				if (args.size() != 4)
				{
					history_.emplace_back("&&Rincorrect number of arguments, need 4!");
					return;
				}

				std::vector<float> nodesConnectionsCount;

				for (const std::string& arg : args)
				{
					try
					{
						for (const std::string& arg : args)
						{
							nodesConnectionsCount.push_back(std::stof(arg));
						}
					}
					catch (const std::exception& e)
					{
						history_.emplace_back("&&Rcan't convert '" + arg + "' to a number: " + e.what());
						return;
					}
				}

				float bestSoFar{};
				float stop{ nodesConnectionsCount[3] };
				sf::Clock stopTimer;
				for (long long i = 0;; i++)
				{
					Graph::get().generateRandomGraph(nodesConnectionsCount[0], nodesConnectionsCount[1], nodesConnectionsCount[2]);
					if (Graph::get().executeAStar() && Graph::get().pathLength() > bestSoFar)
					{
#ifdef _DEBUG
						std::cout << "graphs generated: " << i << '\n';
						std::cout << "path found! path length: " << Graph::get().pathLength() << '\n';
						std::cout << "saving to bestgraph.txt\n";
#endif
						bestSoFar = Graph::get().pathLength();
						std::string command{ "save bestgraph.txt" };
						Console::get().executeCommand(command);
						stop = nodesConnectionsCount[3];
					}
					history_.clear();
					stop -= stopTimer.restart().asSeconds();

					if (stop <= 0.f)
					{
						return;
					}
				}
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
		else if (c == 8)
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

		if (std::ranges::count(command, ' ') > 0)
		{
			size_t pos = 0;
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
				std::get<1>(*pos)({});
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
		else if (!currentText_.empty())
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
			rect.setFillColor(sf::Color(0, 0, 0, 220));
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
			else if (!left && carriagePos_ <= currentText_.size())
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
