#pragma once

#include <SFML/Graphics.hpp>
#include "Connection.hpp"

namespace astar
{
	class Node
	{
	public:
		Node() = default;
		Node(const float x, const float y);
		sf::Vector2f getPos() const;
		bool isMouseOver(const sf::Vector2f& mousePos) const;
		float getDistanceFromMouse(const sf::Vector2f mousePos) const;
		bool isCollision_ = false;
		static const float radius_;
		std::vector<Connection> connections_;
	private:
		float x_;
		float y_;
	};

}
