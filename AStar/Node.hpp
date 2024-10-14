#pragma once

#include <SFML/Graphics.hpp>


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

		static const float radius_;
	private:
		float x_;
		float y_;
	};
}
