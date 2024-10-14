#include "Node.hpp"


namespace astar
{
	Node::Node(const float x, const float y) : x_{ x }, y_{ y }
	{
	}

	sf::Vector2f Node::getPos() const
	{
		return { x_, y_ };
	}

	bool Node::isMouseOver(const sf::Vector2f& mousePos) const
	{
		return std::sqrtf(std::fabs(std::powf(mousePos.x - x_, 2) - std::powf(mousePos.y - y_, 2))) <= radius_;
	}

	float Node::getDistanceFromMouse(const sf::Vector2f mousePos) const
	{
		return std::sqrtf(std::fabs(std::powf(mousePos.x - x_, 2) + std::powf(mousePos.y - y_, 2)));
	}

	const float Node::radius_{ 38.f };
}
