#include "Graph.hpp"
#include "Connection.hpp"
#include <iostream>
#include "Timer.hpp"
#include "Utils.hpp"
#include <random>
#include "Console.hpp"


namespace
{
	bool operator==(const std::pair<int, int>& conn1, const std::pair<int, int>& conn2)
	{
		return (conn1.first == conn2.first && conn1.second == conn2.second) || (conn1.first == conn2.second && conn1.second == conn2.first);
	}
}

namespace astar
{
	Graph& Graph::get()
	{
		static Graph graph;
		return graph;
	}

	void Graph::resetIndex()
	{
		if (nodesCached_.empty()) return;

		int maxId = nodesCached_.front().id();
		for (const auto& node : nodesCached_)
		{
			maxId = maxId < node.id() ? node.id() : maxId;
		}

		freeInd_ = maxId;
	}

	bool Graph::addNode(const sf::Vector2f& pos, const int id, const bool collision)
	{
		if (nodeWithIdExists(id)) return false;
		nodesCached_.emplace_back(pos.x, pos.y, id < 0 ? ++freeInd_ : id, collision);
		return true;
	}

	void Graph::addNodeForce(const sf::Vector2f pos, const int id)
	{
		nodesCached_.emplace_back(pos.x, pos.y, id < 0 ? ++freeInd_ : id, false);
	}

	void Graph::increaseOffset(const float offset)
	{
		offset_ += offset;
	}

	void Graph::selectNodes(const sf::Vector2f& mousePos)
	{
		Node* getNodeFromMouse = checkMouseOnSomething(mousePos);
		if (!startTarget_)
		{
			startTarget_ = getNodeFromMouse;
		}
		else if (!endTarget_)
		{
			endTarget_ = getNodeFromMouse;
		}
		else
		{
			startTarget_ = endTarget_ = nullptr;
		}
	}

	bool Graph::isBuildConnectionMode() const
	{
		return buildConnectionMode_;
	}

	void Graph::toggleConnectionMode()
	{
		connectionText_.setString((buildConnectionMode_ = !buildConnectionMode_) ? "Connection Mode: True\n" : "Connection Mode: False\n");
	}

	void Graph::toggleDrawScore()
	{
		drawScore_ = !drawScore_;
	}

	bool Graph::setStart(const int id)
	{
		for (auto& node : nodesCached_)
		{
			if (node.id() == id)
			{
				if (endTarget_ && endTarget_->id() != id)
				{
					startTarget_ = &node;
					return true;
				}
				else if (!endTarget_)
				{
					startTarget_ = &node;
					return true;
				}
				else
				{
					return false;
				}
			}
		}

		return false;
	}

	bool Graph::setEnd(const int id)
	{
		for (auto& node : nodesCached_)
		{
			if (node.id() == id)
			{
				if (startTarget_ && startTarget_->id() != id)
				{
					endTarget_ = &node;
					return true;
				}
				else if (!startTarget_)
				{
					endTarget_ = &node;
					return true;
				}
				else
				{
					return false;
				}
			}
		}

		return false;
	}

	const std::vector<std::pair<int, int>>& Graph::connectionsCRef() const
	{
		return connections_;
	}

	const std::vector<Connection>& Graph::connectionsCachedCRef() const
	{
		return connectionsCached_;
	}

	const std::vector<Node>& Graph::nodesCRef() const
	{
		return nodesCached_;
	}

	std::optional<std::string> Graph::executeAStar()
	{
		Graph::get().setAStarResult(false, 0.f);
		sf::Clock clk;
		if (!startTarget_ || !endTarget_)
		{
			return std::nullopt;
		}

		for (auto& node : nodesCached_)
		{
			node.fScore_ = std::numeric_limits<float>::max();
			node.gScore_ = std::numeric_limits<float>::max();
			node.parent_ = nullptr;
		}

		for (auto& connection : connectionsCached_)
		{
			connection.line_.setFillColor(sf::Color(200, 200, 200));

			if (!connection.end_->isCollision())
			{
				connection.end_->circle_.setOutlineColor(sf::Color::White);
			}

			if (!connection.start_->isCollision())
			{
				connection.start_->circle_.setOutlineColor(sf::Color::White);
			}
		}

		std::vector<Node*> openSet{ startTarget_ };
		startTarget_->gScore_ = 0;
		startTarget_->fScore_ = utils::euclidDistance(startTarget_->pos(), endTarget_->pos());

		while (!openSet.empty())
		{
			Node* current{ openSet.back() };

			if (current == endTarget_)
			{
				Graph::get().setAStarResult(true, current->fScore_);

				while (current->parent_)
				{
					for (auto& connection : connectionsCached_)
					{
						if ((connection.end_ == current || connection.end_ == current->parent_) && (connection.start_ == current || connection.start_ == current->parent_))
						{
							connection.line_.setFillColor(sf::Color::Blue);
							connection.end_->circle_.setOutlineColor(sf::Color::Blue);
							connection.start_->circle_.setOutlineColor(sf::Color::Blue);
						}
					}

					current = current->parent_;
				}

				return std::to_string(clk.restart().asSeconds());
			}

			openSet.pop_back();

			for (Node* neighbor : current->connections_)
			{
				if (neighbor->isCollision()) continue;

				const float tScore = current->gScore_ + utils::euclidDistance(current->pos(), neighbor->pos());

				if (tScore < neighbor->gScore_)
				{
					neighbor->parent_ = current;
					neighbor->gScore_ = tScore;
					neighbor->fScore_ = tScore + utils::euclidDistance(neighbor->pos(), endTarget_->pos());

					if (std::ranges::find(openSet, neighbor) == openSet.end())
					{
						openSet.push_back(neighbor);
					}
				}
			}

			std::ranges::sort(openSet, [](const Node* left, const Node* right) { return left->fScore_ > right->fScore_; });
		}

		return std::nullopt;
	}

	void Graph::toggleRapidConnect()
	{
		rapidConnect_ = !rapidConnect_;
	}

	bool Graph::isRapidConnect() const
	{
		return rapidConnect_;
	}

	void Graph::generateRandomGraph(const int nodesCount, const float chance, const float radius)
	{
		const float originalChance{ chance };
		constexpr int maxTries{ 10000 };
		resetNodes();
		std::uniform_real_distribution<float> dist(-radius, radius);
		std::uniform_real_distribution<float> p(0.f, 1.f);
		std::uniform_int_distribution<int> pathLength(2, nodesCount - 1);
		static std::mt19937_64 mt(std::random_device{}());

		sf::Vector2f pos{ dist(mt), dist(mt) };

		int tries{ 0 };
		for (int i = 0; i < nodesCount; ++i)
		{
			bool valid = true;
			while (!isValidPosition(pos, radius) && ++tries < maxTries)
			{
				valid = false;
				pos = { dist(mt), dist(mt) };
				valid = true;
			}

			if (valid && tries != maxTries)
			{
				addNodeForce(pos);
			}

			tries = 0;
		}

		std::vector<int> pathIds;
		//pathIds.resize(pathLength(mt));
		//
		//std::string command{ "path 1" };
		//
		//for (int& id : pathIds)
		//{
		//	id = pathLength(mt);
		//}

		pathIds.resize(nodesCount);

		for (int i = 0; i < pathIds.size(); i++)
		{
			pathIds[i] = i + 1;
		}

		//std::ranges::sort(pathIds);

		//pathIds.erase(std::unique(pathIds.begin(), pathIds.end()), pathIds.end());

		//for (const int id : pathIds)
		//{
		//	command += " " + std::to_string(id);
		//}

		//long iter = 0;
		for (int i = 1; i <= nodesCount; i++)
		{
			for (int j = i+1; j <= nodesCount; j++)
			{
				//iter++;
				if (p(mt) < chance)
				{
					addIdConnectionForce(i, j);
					//chance = originalChance;
				}
			}
		}

		//std::cout << chance << '\n';
		//std::cout << "ITERATIONS: " << iter << '\n';

		//command += " " + std::to_string(nodesCount);

		//std::string linkCommand{ "link" };
		std::string setStart{ "set start 1" };
		std::string setEnd{ "set end " + std::to_string(nodesCount) };
		Console::get().executeCommand(setStart);
		Console::get().executeCommand(setEnd);
		//Console::get().executeCommand(command);
	}

	bool Graph::isValidPosition(const sf::Vector2f pos, const float radius)
	{
		if (utils::euclidDistance({ 0,0 }, pos) > radius)
		{
			return false;
		}

		constexpr float distSquared{ 68.f * 68.f };
		for (const Node& node : nodesCached_)
		{
			if (utils::euclidDistanceSquared(node.pos(), pos) <= distSquared)
			{
				return false;
			}
		}

		return true;
	}

	void Graph::draw(const sf::Vector2f& mousePos)
	{
		if (savedNode_)
		{
			sf::RectangleShape rect({ savedNode_->distanceFromMouse(mousePos), 5 });
			rect.setOrigin(0, 2.5);
			rect.setRotation(utils::getAngleDeg(mousePos, savedNode_->pos()));
			rect.setPosition(savedNode_->pos());
			rt_->draw(rect);
		}

		for (const auto& connection : connectionsCached_)
		{
			rt_->draw(connection.line_);
		}

		for (const auto& node : nodesCached_)
		{
			rt_->draw(node.circle_);

			if (drawDistance_)
			{
				text_.setPosition(node.pos().x - 35.f, node.pos().y + 32.f);
				text_.setString(std::to_string(node.distanceFromMouse(mousePos)));
				text_.setFillColor(sf::Color::White);
				rt_->draw(text_);
			}

			if (drawIds_)
			{
				const std::string nodeId = std::to_string(node.id());
				const float width = nodeId.size() * offset_ / 2.f;
				text_.setPosition(node.pos().x - width, node.pos().y - 14.f);
				text_.setString(nodeId);
				text_.setFillColor(sf::Color::Black);
				text_.setCharacterSize(24);
				rt_->draw(text_);
				text_.setCharacterSize(16);
			}

			if (drawScore_)
			{
				text_.setPosition(node.pos().x - 100.f, node.pos().y + 30.f);
				text_.setString(std::format("fScore: {}\ngScore: {}", node.fScore_, node.gScore_));
				text_.setFillColor(sf::Color::White);
				rt_->draw(text_);
			}
		}
	}

	void Graph::drawStats()
	{
		connectionText_.setString(buildConnectionMode_ ? "Connection Mode: True\n" : "Connection Mode: False\n");

		if (buildConnectionMode_)
		{
			connectionText_.setString(connectionText_.getString() + (rapidConnect_ ? "Rapid Connect: True\n" : "Rapid Connect: False\n"));
		}

		if (startTarget_)
		{
			connectionText_.setString(connectionText_.getString() + "Start Target: " + std::to_string(startTarget_->id()) + '\n');
		}

		if (endTarget_)
		{
			connectionText_.setString(connectionText_.getString() + "End Target: " + std::to_string(endTarget_->id()));
		}

		rt_->draw(connectionText_);
		connectionText_.setString("");
	}

	void Graph::setCollision(const sf::Vector2f& mousePos)
	{
		for (auto& node : nodesCached_)
		{
			if (node.isMouseOver(mousePos))
			{
				node.toggleCollision();
				break;
			}
		}
	}

	void Graph::clearSavedNode()
	{
		savedNode_ = nullptr;
	}

	void Graph::moveNode(const sf::Vector2f mousePos)
	{
		Node* checkMouseUp = checkMouseOnSomething(mousePos);
		if (checkMouseUp && !savedNode_)
		{
			checkMouseUp->changePos(mousePos);
			savedNode_ = checkMouseUp;
		}
		else if (savedNode_)
		{
			savedNode_->changePos(mousePos);
		}
	}

	void Graph::checkAndDelete(const sf::Vector2f& mousePos)
	{
		for (auto& nd : nodesCached_)
		{
			if (nd.isMouseOver(mousePos))
			{
				if (&nd == startTarget_)
				{
					startTarget_ = nullptr;
				}
				else if (&nd == endTarget_)
				{
					endTarget_ = nullptr;
				}

				deleteNode(nd.id());
				handleRecalculate();

				break;
			}
		}

	}

	void Graph::toggleDrawDistance()
	{
		drawDistance_ = !drawDistance_;
	}

	void Graph::setDrawIds(const bool drawIds)
	{
		drawIds_ = drawIds;
	}

	Node* Graph::checkMouseOnSomething(const sf::Vector2f& mousePos)
	{
		for (auto& node : nodesCached_)
		{
			if (node.isMouseOver(mousePos))
			{
				return &node;
			}
		}
		return nullptr;
	}

	void Graph::makeConnection(const sf::Vector2f& mousePos)
	{
		for (auto& node : nodesCached_)
		{
			if (node.isMouseOver(mousePos))
			{
				if (savedNode_ && savedNode_ != &node)
				{
					if (connectionExists({ savedNode_->id(), node.id() }))
					{
						return;
					}

					savedNode_->connections_.emplace_back(&node);
					node.connections_.emplace_back(savedNode_);
					connections_.emplace_back(savedNode_->id(), node.id());

					connectionsCached_.emplace_back(savedNode_,
						&node,
						1,
						utils::euclidDistance(savedNode_->pos(), node.pos()),
						utils::getAngleDeg(node.pos(), savedNode_->pos()));

#ifdef _DEBUG
					for (const auto& [left, right] : connections_)
					{
						std::cout << std::format("{}<->{}\n", left, right);
					}
					std::cout << "------------------------------\n";
#endif


					savedNode_ = &node;
				}
				else
				{
					savedNode_ = &node;
				}
				break;
			}
		}
	}

	void Graph::addIdConnectionForce(const int id1, const int id2)
	{
		connections_.push_back({ id1,id2 });

		for (Node& nodeL : nodesCached_)
		{
			if (nodeL.id() == id1)
			{
				for (Node& nodeR : nodesCached_)
				{
					if (nodeR.id() == id2)
					{
						nodeL.connections_.emplace_back(&nodeR);
						nodeR.connections_.emplace_back(&nodeL);

						connectionsCached_.emplace_back(
							&nodeR,
							&nodeL,
							1,
							utils::euclidDistance(nodeL.pos(), nodeR.pos()),
							utils::getAngleDeg(nodeL.pos(), nodeR.pos()));

						return;
					}
				}
			}
		}
	}

	bool Graph::addIdConnection(const std::pair<int, int>& connection)
	{
		if (connection.first == connection.second) return false;

		if (!connectionExists(connection))
		{
			connections_.push_back(connection);

			for (Node& nodeL : nodesCached_)
			{
				if (nodeL.id() == connection.first)
				{
					for (Node& nodeR : nodesCached_)
					{
						if (nodeR.id() == connection.second)
						{
							nodeL.connections_.emplace_back(&nodeR);
							nodeR.connections_.emplace_back(&nodeL);

							connectionsCached_.emplace_back(
								&nodeR,
								&nodeL,
								1,
								utils::euclidDistance(nodeL.pos(), nodeR.pos()),
								utils::getAngleDeg(nodeL.pos(), nodeR.pos()));

							return true;
						}
					}
				}
			}
		}

		return false;
	}

	void Graph::resetNodes()
	{
		startTarget_ = nullptr;
		endTarget_ = nullptr;
		savedNode_ = nullptr;
		nodesCached_.clear();
		connectionsCached_.clear();
		connections_.clear();
		freeInd_ = 0;
	}

	void Graph::deleteNode(const int id)
	{
		std::erase_if(connections_, [id](const std::pair<int, int>& con) { return con.first == id || con.second == id; });

		std::erase_if(nodesCached_, [id](const Node& node)
			{
#ifdef _DEBUG
				std::cout << "node.id(): " << node.id() << ", arg id: " << id << '\n';
#endif
				return node.id() == id;
			});

		handleRecalculate();
	}

	bool Graph::nodeWithIdExists(const int id) const
	{
		return std::ranges::find_if(nodesCached_, [&id](const Node& node) { return node.id() == id; }) != nodesCached_.end();
	}

	bool Graph::connectionExists(const std::pair<int, int>& connection) const
	{
		return std::ranges::any_of(connections_, [&connection](const std::pair<int, int>& conn)
			{
				return conn == connection;
			});
	}

	void Graph::handleRecalculate()
	{
		for (auto& nd : nodesCached_)
		{
			nd.connections_.clear();
		}

		connectionsCached_.clear();

		for (const auto& [left, right] : connections_)
		{
			for (Node& nodeL : nodesCached_)
			{
				if (nodeL.id() == left)
				{
					for (Node& nodeR : nodesCached_)
					{
						if (nodeR.id() == right)
						{
							nodeL.connections_.emplace_back(&nodeR);
							nodeR.connections_.emplace_back(&nodeL);

							connectionsCached_.emplace_back(
								&nodeR,
								&nodeL,
								1,
								utils::euclidDistance(nodeL.pos(), nodeR.pos()),
								utils::getAngleDeg(nodeL.pos(), nodeR.pos()));
						}
					}
				}
			}
		}
	}

	void Graph::setRenderTarget(sf::RenderTarget* rt)
	{
		rt_ = rt;
	}

	void Graph::setAStarResult(const bool result, const float pathLength)
	{
		pathFound_ = result;
		pathLength_ = pathLength;
	}

	float Graph::pathLength() const
	{
		return pathLength_;
	}

	Graph::Graph() : drawDistance_{ false }, savedNode_{}, freeInd_{}, shouldRecalculate_{}, offset_{ 15.f },
					 drawIds_{}, startTarget_{}, endTarget_{}, buildConnectionMode_{}, rapidConnect_{}, drawScore_{}, rt_{}, pathLength_{}, pathFound_{}
	{
		nodesCached_.reserve(10000);
		connectionsCached_.reserve(20000);
		connections_.reserve(20000);
		font_.loadFromFile("mono.ttf");
		connectionText_.setFont(font_);
		connectionText_.setString("Connection Mode: False");
		connectionText_.setPosition(5, 15);
		text_.setCharacterSize(16);
		text_.setFont(font_);
	}
}
