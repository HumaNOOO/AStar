#pragma once

#include <SFML/Graphics.hpp>
#include "Connection.hpp"

namespace astar
{
	class Node
	{
	public:
		Node();
		Node(const float x, const float y, const int id, const bool collision = false);
		Node(const Node& other) = default;
		sf::Vector2f pos() const;
		bool isMouseOver(const sf::Vector2f& mousePos) const;
		float distanceFromMouse(const sf::Vector2f mousePos) const;
		int id() const;
		void toggleCollision();
		bool isCollision() const;
		static constexpr float radius_{ 26.f };
		static constexpr float border_{ 4.f };
		std::vector<Node*> connections_;
		sf::CircleShape circle_;
		void changePos(const sf::Vector2f& mousePos);
		Node& operator=(const Node& other);
		Node& operator=(Node&& other) noexcept;
		float gScore_;
		float fScore_;
		Node* parent_;

	private:
		int id_;
		bool isCollision_;
	};
}
