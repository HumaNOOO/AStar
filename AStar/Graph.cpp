#include "Graph.hpp"
#include "Connection.hpp"
#include <iostream>
#include "Timer.hpp"


namespace
{
	bool operator==(const std::pair<int, int>& conn1, const std::pair<int, int>& conn2)
	{
		return (conn1.first == conn2.first && conn1.second == conn2.second) || (conn1.first == conn2.second && conn1.second == conn2.first);
	}
}

namespace astar
{
	Graph& Graph::getInstance()
	{
		static Graph graph;
		return graph;
	}

	void Graph::resetIndex()
	{
		long maxId = nodesCached_.front().id();
		for (const auto& node : nodesCached_)
		{
			maxId = maxId < node.id() ? node.id() : maxId;
		}

		freeInd_ = maxId;
	}

	void Graph::addNode(const float x, const float y)
	{
		nodesCached_.emplace_back(x, y, freeInd_++);
	}

	void Graph::addNode(const sf::Vector2f& pos, const int id)
	{
		nodesCached_.emplace_back(pos.x, pos.y, id == -1 ? ++freeInd_ : id);
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
		connectionText_.setString((buildConnectionMode_ = !buildConnectionMode_) ? "Connection Mode: True" : "Connection Mode: False");
	}

	const std::vector<std::pair<int, int>>& Graph::getConnectionsCRef() const
	{
		return connections_;
	}

	void Graph::draw(sf::RenderTarget& rt, const sf::Vector2f& mousePos)
	{
		if (startTarget_)
		{
			connectionText_.setString(connectionText_.getString() + "\nStart Target: " + std::to_string(startTarget_->id()));
		}

		if (endTarget_)
		{
			connectionText_.setString(connectionText_.getString() + "\nEnd Target: " + std::to_string(endTarget_->id()));
		}

		rt.draw(connectionText_);

		for (const auto& node : nodesCached_)
		{
			for (const auto& connection : node.connections_)
			{
				if (connection.render_)
				{
					const float distance = std::sqrtf(std::fabs(std::powf(node.getPos().x - connection.end_->getPos().x, 2) + std::powf(node.getPos().y - connection.end_->getPos().y, 2)));
					sf::RectangleShape line(sf::Vector2f(distance, 5));
					line.setRotation(getAngleDeg(node.getPos(), connection.end_->getPos()));
					line.setOrigin(0, 2.5);
					line.setPosition(connection.end_->getPos());
					rt.draw(line);
				}
			}
		}

		if (savedNode_)
		{
			sf::RectangleShape rect({ savedNode_->getDistanceFromMouse(mousePos), 5 });
			rect.setOrigin(0, 2.5);
			rect.setRotation(getAngleDeg(mousePos, savedNode_->getPos()));
			rect.setPosition(savedNode_->getPos());
			rt.draw(rect);
		}

		for (const auto& node : nodesCached_)
		{
			node.draw(rt);

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
				std::erase_if(connections_, [&nd](const std::pair<int, int>& con) { return con.first == nd.id() || con.second == nd.id(); });
				std::erase_if(nodesCached_, [&nd](const Node& node) {return &node == &nd; });

				if (&nd == startTarget_)
				{
					startTarget_ = nullptr;
				}
				else if (&nd == endTarget_)
				{
					endTarget_ = nullptr;
				}
				break;
			}
		}

		handleRecalculate();
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
					for (const auto& connStart : savedNode_->connections_)
					{
						if (savedNode_ == connStart.end_)
						{
							return;
						}
					}

					for (const auto& conn : node.connections_)
					{
						if (savedNode_ == conn.end_)
						{
							return;
						}
					}

					savedNode_->connections_.emplace_back(&node, 1, true);
					node.connections_.emplace_back(savedNode_, 1, false);

					connections_.emplace_back(savedNode_->id(), node.id());

#ifdef _DEBUG
					std::cout << savedNode_ << "->" << &node << '\n';
					std::cout << &node << "->" << savedNode_ << '\n';

					for (const auto& [left, right] : connections_)
					{
						std::cout << std::format("{}->{}\n", left, right);
					}
					std::cout << "------------------------------\n";
#endif


					savedNode_ = nullptr;
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
							node_l.connections_.emplace_back(&node_r, 1, true);
							node_r.connections_.emplace_back(&node_l, 1, false);
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
		nodesCached_.clear();

		connections_.clear();

		freeInd_ = 0;
	}

	void Graph::deleteNode(const int id)
	{
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

	void Graph::update()
	{

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
							node_l.connections_.emplace_back(&node_r, 1, true);
							node_r.connections_.emplace_back(&node_l, 1, false);
						}
					}
				}
			}
		}
	}

	float Graph::getAngleDeg(const sf::Vector2f p1, const sf::Vector2f p2) const
	{
		return std::atan2(p1.y - p2.y, p1.x - p2.x) * 57.30659025f;
	}

	Graph::Graph() : drawDistance_{ false }, savedNode_{}, nodesChanged_{}, freeInd_{}, shouldRecalculate_{}, offset_{ 10.f }, drawIds_{}, startTarget_{}, endTarget_{}, buildConnectionMode_{}
	{
		nodesCached_.reserve(1000);
		font_.loadFromFile("mono.ttf");
		connectionText_.setFont(font_);
		connectionText_.setString("Connection Mode: False");
		connectionText_.setPosition(5, 15);
		text_.setCharacterSize(16);
		text_.setFont(font_);
	}
}
