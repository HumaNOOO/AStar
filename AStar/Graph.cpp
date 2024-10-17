#include "Graph.hpp"
#include "Connection.hpp"
#include <iostream>
#include "Timer.hpp"

namespace astar
{
	Graph& Graph::getInstance()
	{
		static Graph graph;
		return graph;
	}

	void Graph::addNode(const float x, const float y)
	{
		nodesCached_.emplace_back(x, y, freeInd_++);
	}

	void Graph::addNode(const sf::Vector2f& pos)
	{
		nodesCached_.emplace_back(pos.x, pos.y, freeInd_++);
	}

	void Graph::increaseOffset(const float offset) 
	{
		offset_ += offset;
	}

	void Graph::selectNodes(const sf::Vector2f& mousePos) {
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

		sf::CircleShape circle(Node::radius_);

		for (const auto& node : nodesCached_)
		{
			for (const auto& connection : node.connections_)
			{
				if (connection.render_)
				{
					const float angle = std::atan2(node.getPos().y - connection.end_->getPos().y, node.getPos().x - connection.end_->getPos().x) * (180.f / 3.141f);
					const float distance = std::sqrtf(std::fabs(std::powf(node.getPos().x - connection.end_->getPos().x, 2) + std::powf(node.getPos().y - connection.end_->getPos().y, 2)));
					sf::RectangleShape line(sf::Vector2f(distance, 5));
					line.setRotation(angle);
					line.setPosition(connection.end_->getPos().x, connection.end_->getPos().y);
					rt.draw(line);
				}
			}
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
		Node* checkMouseUp = astar::Graph::getInstance().checkMouseOnSomething(mousePos);
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
				for (auto& node : nodesCached_)
				{
					if (&node != &nd) {
						for (int i = 0; i < node.connections_.size(); i++)
						{
							if (node.connections_[i].end_ == &nd)
							{
								node.connections_.erase(node.connections_.begin() + i);
							}
						}
					}
				}
				nd.connections_.clear(); //wtf?
				if (&nd == startTarget_) 
				{
					startTarget_ = nullptr;
				}
				else if (&nd == endTarget_)
				{
					endTarget_ = nullptr;
				}

				std::erase_if(nodesCached_, [&nd](const Node& node) { return &node == &nd; });

				break;
			}
		}
	}

	void Graph::setDrawDistance(const bool drawDistance)
	{
		drawDistance_ = drawDistance;
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
					
					savedNode_->connections_.emplace_back(&node, 1, true); //Cost always 1
					node.connections_.emplace_back(savedNode_, 1, false);
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

	void Graph::resetNodes()
	{
		nodesCached_.clear();
		freeInd_ = 0;
	}

	void Graph::update()
	{

	}

	void Graph::handleRecalculate()
	{

	}

	Graph::Graph() : drawDistance_{ false }, savedNode_{}, nodesChanged_{}, freeInd_{}, shouldRecalculate_{}, offset_{ 10.f }, drawIds_{}, startTarget_{}, endTarget_{}, buildConnectionMode_{}
	{
		nodesCached_.reserve(1000);
		std::cout << "Graph::Graph()" << '\n';
		font_.loadFromFile("mono.ttf");
		connectionText_.setFont(font_);
		connectionText_.setString("Connection Mode: False");
		connectionText_.setPosition(5, 15);
		text_.setCharacterSize(16);
		text_.setFont(font_);
	}
}
