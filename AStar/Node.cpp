#include "Node.hpp"
#include <iostream>
#include <format>


namespace astar
{
	Node::Node(const float x, const float y, const long id) : isCollision_{}, connections_{}, id_{ id }
	{
		circle_.setPosition(x, y);
		circle_.setRadius(radius_);
		circle_.setOrigin(radius_, radius_);
		circle_.setFillColor(sf::Color::Green);
		circle_.setOutlineColor(sf::Color::Blue);
		circle_.setOutlineThickness(border_);

		std::cout << std::format("Creating node at ({},{}) with id={}\n", x, y, id);
	}
	void Node::changePos(const sf::Vector2f& mousepos)
	{
		circle_.setPosition(mousepos.x, mousepos.y);
	}
	const sf::Vector2f& Node::getPos() const
	{
		return circle_.getPosition();
	}

	bool Node::isMouseOver(const sf::Vector2f& mousePos) const
	{
		return std::sqrtf(std::fabs(std::powf(mousePos.x - circle_.getPosition().x, 2) + std::powf(mousePos.y - circle_.getPosition().y, 2))) < (radius_ + border_);
	}

	float Node::getDistanceFromMouse(const sf::Vector2f mousePos) const
	{
		return std::sqrtf(std::fabs(std::powf(mousePos.x - circle_.getPosition().x, 2) + std::powf(mousePos.y - circle_.getPosition().y, 2)));
	}

	long Node::id() const
	{
		return id_;
	}

	void Node::toggleCollision()
	{
		isCollision_ = !isCollision_;

		if (isCollision_)
		{
			circle_.setFillColor(sf::Color::Red);
		}
		else
		{
			circle_.setFillColor(sf::Color::Green);
		}
	}

	bool Node::isCollision() const
	{
		return isCollision_;
	}

	void Node::draw(sf::RenderTarget& rt) const
	{
		rt.draw(circle_);
	}

	Node& Node::operator=(const Node& other)
	{
		if (this != &other)
		{
			id_ = other.id_;
			circle_ = other.circle_;
			//std::cout << "Node::operator=(const Node& other) called\n";
		}
		return *this;
	}

	Node& Node::operator=(Node&& other) noexcept
	{
		if (this != &other)
		{
			id_ = std::move(other.id_);
			circle_ = std::move(other.circle_);
			//std::cout << "Node::operator=(Node&& other) called\n";
		}
		return *this;
	}
}
