#include "Node.hpp"
#include <iostream>
#include <format>
#include "Graph.hpp"

namespace astar
{
	Node::Node() : id_{}, isCollision_{}, gScore_{}, fScore_{}, parent_{}
	{
	}

	Node::Node(const float x, const float y, const int id, const bool collision) : isCollision_{ collision }, connections_{}, id_{ id }, gScore_{ std::numeric_limits<float>::max() }, fScore_{ std::numeric_limits<float>::max() }, parent_{ nullptr }
	{
		circle_.setPosition(x, y);
		circle_.setRadius(radius_);
		circle_.setOrigin(radius_, radius_);
		circle_.setFillColor(sf::Color::Green);
		circle_.setOutlineColor(collision ? sf::Color::Red : sf::Color::White);
		circle_.setOutlineThickness(border_);

#ifdef _DEBUG
		std::cout << std::format("Creating node at ({},{}) with id={}\n", x, y, id);
#endif
	}

	void Node::changePos(const sf::Vector2f& mousePos)
	{
		circle_.setPosition(mousePos.x, mousePos.y);
	}

	sf::Vector2f Node::getPos() const
	{
		return circle_.getPosition();
	}

	bool Node::isMouseOver(const sf::Vector2f& mousePos) const
	{
		return getDistanceFromMouse(mousePos) < (radius_ + border_ + (astar::Graph::get().isRapidConnect() ? 30.f : 0.f));
	}

	float Node::getDistanceFromMouse(const sf::Vector2f mousePos) const
	{
		return std::sqrtf(std::powf(mousePos.x - circle_.getPosition().x, 2) + std::powf(mousePos.y - circle_.getPosition().y, 2));
	}

	int Node::id() const
	{
		return id_;
	}

	void Node::toggleCollision()
	{
		circle_.setOutlineColor((isCollision_ = !isCollision_) ? sf::Color::Red : sf::Color::White);
	}

	bool Node::isCollision() const
	{
		return isCollision_;
	}

	Node& Node::operator=(const Node& other)
	{
		if (this != &other)
		{
			id_ = other.id_;
			connections_ = other.connections_;
			isCollision_ = other.isCollision_;
			circle_ = other.circle_;
		}
		return *this;
	}

	Node& Node::operator=(Node&& other) noexcept
	{
		if (this != &other)
		{
			id_ = std::move(other.id_);
			connections_ = std::move(other.connections_);
			isCollision_ = std::move(other.isCollision_);
			circle_ = std::move(other.circle_);
		}
		return *this;
	}
}
