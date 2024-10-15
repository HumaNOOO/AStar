#pragma once

#include <SFML/Graphics.hpp>
#include "Connection.hpp"

namespace astar
{
	class Node
	{
	public:
		Node() = default;
		Node(const float x, const float y, const long id);
		Node(const Node& other) = default;
		const sf::Vector2f& getPos() const;
		bool isMouseOver(const sf::Vector2f& mousePos) const;
		float getDistanceFromMouse(const sf::Vector2f mousePos) const;
		long id() const;
		void toggleCollision();
		bool isCollision() const;
		void draw(sf::RenderTarget& rt) const;
		static constexpr float radius_{ 26.f };
		static constexpr float border_{ 4.f };
		std::vector<Connection> connections_;
		sf::CircleShape circle_;
		void changePos(const sf::Vector2f& mousePos);
		Node& operator=(const Node& other);
		Node& operator=(Node&& other) noexcept;

	private:
		long id_;
		bool isCollision_;
	};
}
