#include "Graph.hpp"

#include <iostream>


namespace astar
{
	Graph& Graph::getInstance()
	{
		static Graph graph;
		return graph;
	}

	void Graph::addNode(const float x, const float y)
	{
		nodes_.emplace_back(x, y);
	}

	void Graph::addNode(const sf::Vector2f& pos)
	{
		nodes_.emplace_back(pos.x, pos.y);
	}

	void Graph::draw(sf::RenderTarget& rt, const sf::Vector2f& mousePos) const
	{
		sf::CircleShape circle(Node::radius_ - 2.f);
		sf::Text text;
		text.setFont(font_);

		for (const auto& node : nodes_)
		{
			circle.setFillColor(sf::Color::Green);
			circle.setOutlineColor(sf::Color::Blue);
			circle.setOutlineThickness(5);
			circle.setPosition(node.getPos());
			circle.setOrigin(Node::radius_, Node::radius_);
			rt.draw(circle);

			if (drawDistance_)
			{
				text.setPosition(node.getPos());
				text.setString(std::to_string(node.getDistanceFromMouse(mousePos)));
				rt.draw(text);
			}
		}
	}

	void Graph::checkAndDelete(const sf::Vector2f& mousePos)
	{
		for (const auto& node : nodes_)
		{
			if (node.isMouseOver(mousePos))
			{
				std::erase_if(nodes_, [addr = &node](const Node& node) { return &node == addr; });
				break;
			}
		}
	}

	void Graph::setDrawDistance(const bool drawDistance)
	{
		drawDistance_ = drawDistance;
	}

	Graph::Graph() : drawDistance_{ false }
	{
		std::cout << "Graph::Graph()\n";
		font_.loadFromFile("mono.ttf");
	}
}
