#include "Graph.hpp"
#include "Connection.hpp"
#include <iostream>
#include "Timer.hpp"
#include "Utils.hpp"


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
		nodesCached_.emplace_back(pos.x, pos.y, id == -1 ? ++freeInd_ : id, collision);
		return true;
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
			startTarget_ = nullptr;
			endTarget_ = nullptr;
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

	const std::vector<std::pair<int, int>>& Graph::getConnectionsCRef() const
	{
		return connections_;
	}

	const std::vector<Node>& Graph::getNodesCRef() const
	{
		return nodesCached_;
	}

	void Graph::drawPath()
	{

	}

	void Graph::executeAStar()
	{
		if (!startTarget_ || !endTarget_)
		{
			return;
		}

		for (auto& connection : connectionsCached_)
		{
			connection.line_.setFillColor(sf::Color::White);

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
		startTarget_->fScore_ = utils::euclidDistance(startTarget_->getPos(), endTarget_->getPos());

		while (!openSet.empty())
		{
			Node* current = openSet.back();

			if (current == endTarget_)
			{
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

				return;
			}

			openSet.pop_back();

			for (Node* neighbor : current->connections_)
			{
				if (neighbor->isCollision()) continue;

				const float tScore = current->gScore_ + utils::euclidDistance(current->getPos(), neighbor->getPos());

				if (tScore < neighbor->gScore_)
				{
					neighbor->parent_ = current;
					neighbor->gScore_ = tScore;
					neighbor->fScore_ = tScore + utils::euclidDistance(neighbor->getPos(), endTarget_->getPos());

					if (std::ranges::find(openSet, neighbor) == openSet.end())
					{
						openSet.push_back(neighbor);
					}
				}
			}

			std::ranges::sort(openSet, [](const Node* left, const Node* right) { return left->fScore_ > right->fScore_; });
		}
	}

	void Graph::toggleRapidConnect()
	{
		rapidConnect_ = !rapidConnect_;
	}

	bool Graph::isRapidConnect() const
	{
		return rapidConnect_;
	}

	void Graph::draw(sf::RenderTarget& rt, const sf::Vector2f& mousePos)
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

		if (savedNode_)
		{
			sf::RectangleShape rect({ savedNode_->getDistanceFromMouse(mousePos), 5 });
			rect.setOrigin(0, 2.5);
			rect.setRotation(utils::getAngleDeg(mousePos, savedNode_->getPos()));
			rect.setPosition(savedNode_->getPos());
			rt.draw(rect);
		}

		for (const auto& connection : connectionsCached_)
		{
			rt.draw(connection.line_);
		}

		for (const auto& node : nodesCached_)
		{
			rt.draw(node.circle_);

			if (drawDistance_)
			{
				text_.setPosition(node.getPos().x - 35.f, node.getPos().y + 32.f);
				text_.setString(std::to_string(node.getDistanceFromMouse(mousePos)));
				text_.setFillColor(sf::Color::White);
				rt.draw(text_);
			}

			if (drawIds_)
			{
				const std::string nodeId = std::to_string(node.id());
				const float width = nodeId.size() * offset_ / 2.f;
				text_.setPosition(node.getPos().x - width, node.getPos().y - 8.f);
				text_.setString(nodeId);
				text_.setFillColor(sf::Color::Black);
				rt.draw(text_);
			}
		}

		rt.draw(connectionText_);
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
						astar::utils::euclidDistance(savedNode_->getPos(), node.getPos()),
						astar::utils::getAngleDeg(node.getPos(), savedNode_->getPos()));

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

	bool Graph::addIdConnection(const std::pair<int, int>& connection)
	{
		if (!connectionExists(connection))
		{
			connections_.push_back(connection);

			for (Node& node_l : nodesCached_)
			{
				if (node_l.id() == connection.first)
				{
					for (Node& node_r : nodesCached_)
					{
						if (node_r.id() == connection.second)
						{
							node_l.connections_.emplace_back(&node_r);
							node_r.connections_.emplace_back(&node_l);

							connectionsCached_.emplace_back(
								&node_r,
								&node_l,
								1,
								utils::euclidDistance(node_l.getPos(), node_r.getPos()),
								utils::getAngleDeg(node_l.getPos(), node_r.getPos()));

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
		std::erase_if(connectionsCached_, [id](const Connection& con) { return con.start_->id() == id || con.end_->id() == id; });

		std::erase_if(nodesCached_, [id](const Node& node)
			{
#ifdef _DEBUG
				std::cout << "node.id(): " << node.id() << ", arg id: " << id << '\n';
#endif
				return node.id() == id;
			});

		std::erase_if(connections_, [id](const std::pair<int, int>& con) { return con.first == id || con.second == id; });

		handleRecalculate();
	}

	bool Graph::nodeWithIdExists(const int id) const
	{
		return std::ranges::find_if(nodesCached_, [&id](const Node& node) { return node.id() == id; }) != nodesCached_.end();
	}

	bool Graph::connectionExists(const std::pair<int, int>& connection) const
	{
		return !std::ranges::none_of(connections_, [&connection](const std::pair<int, int>& conn)
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

		for (const auto& [left, right] : connections_)
		{
			for (Node& node_l : nodesCached_)
			{
				if (node_l.id() == left)
				{
					for (Node& node_r : nodesCached_)
					{
						if (node_r.id() == right)
						{
							node_l.connections_.emplace_back(&node_r);
							node_r.connections_.emplace_back(&node_l);

							connectionsCached_.emplace_back(
								&node_r,
								&node_l,
								1,
								astar::utils::euclidDistance(node_l.getPos(), node_r.getPos()),
								astar::utils::getAngleDeg(node_l.getPos(), node_r.getPos()));
						}
					}
				}
			}
		}
	}

	Graph::Graph() : drawDistance_{ false }, savedNode_{}, freeInd_{}, shouldRecalculate_{}, offset_{ 10.f }, drawIds_{}, startTarget_{}, endTarget_{}, buildConnectionMode_{}, rapidConnect_{}
	{
		nodesCached_.reserve(200);
		connectionsCached_.reserve(200);
		font_.loadFromFile("mono.ttf");
		connectionText_.setFont(font_);
		connectionText_.setString("Connection Mode: False");
		connectionText_.setPosition(5, 15);
		text_.setCharacterSize(16);
		text_.setFont(font_);
	}
}
