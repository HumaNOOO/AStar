#include "Graph.hpp"
#include "Connection.hpp"
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

	void Graph::draw(sf::RenderTarget& rt, const sf::Vector2f& mousePos,bool makeConnection) const
	{
		sf::Text modeBlock;
		if(makeConnection)
		{
			modeBlock.setString("Connection Mode: True");
		}
		else 
		{
			modeBlock.setString("Connection Mode: False");
		}
		modeBlock.setPosition(0,15);
		modeBlock.setFont(font_);
		rt.draw(modeBlock);
		sf::CircleShape circle(Node::radius_ - 2.f);
		sf::Text text;
		text.setFont(font_);
		for (const auto& node : nodes_) {
			for (const auto& connection : node.connections_) {
				if (connection.render_)
				{
					float angle = atan2(node.getPos().y - connection.end_->getPos().y, node.getPos().x - connection.end_->getPos().x);
					angle = angle * (180 / 3.141);
					float distance = std::sqrtf(std::fabs(std::powf(node.getPos().x - connection.end_->getPos().x, 2) + std::powf(node.getPos().y - connection.end_->getPos().y, 2)));
					sf::RectangleShape line(sf::Vector2f(distance, 5));
					line.setRotation(angle);
					line.setPosition(connection.end_->getPos().x, connection.end_->getPos().y);
					rt.draw(line);
				}
			} //render lines
		}

		for (const auto& node : nodes_)
		{

			if (node.isCollision_ == false)
				circle.setFillColor(sf::Color::Green);
			else
				circle.setFillColor(sf::Color::Red);
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
	void Graph::setCollision(const sf::Vector2f& mousePos) {
		for (auto& node : nodes_)
		{
			if (node.isMouseOver(mousePos))
			{
				node.isCollision_ = !node.isCollision_;
				break;
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

	void Graph::makeConnection(sf::Vector2f& mousepos) {
		for (auto& node : nodes_) {
			if (node.isMouseOver(mousepos)) {
				
				if (savedNode_ && savedNode_ != &node) {
					for (auto& node2 : nodes_) {
						for (auto& conn : node.connections_) {
							if (savedNode_ == conn.end_) {
								return;
							}
						}
						for (auto& conn : node2.connections_) {
							if (savedNode_ == conn.end_) {
								return;
							}
						}
					}
					savedNode_->connections_.emplace_back(&node, 1,true); //Cost always 1
					node.connections_.emplace_back(savedNode_, 1,false);
					savedNode_ = nullptr;
				}
				else {
					savedNode_ = &node;
				}
				break;
			}
		}
	}

	Graph::Graph() : drawDistance_{ false }
	{
		nodes_.reserve(1000);
		std::cout << "Graph::Graph()\n";
		font_.loadFromFile("mono.ttf");
	}
}
