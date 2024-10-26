#include "Connection.hpp"
#include <format>
#include <iostream>

#include "Node.hpp"

namespace astar
{
	Connection::Connection(Node* start, Node* end, unsigned int cost, const float distance, const float angle) : start_{ start }, end_{ end }, cost_{ cost }
	{
		line_.setSize({ distance, 5 });
		line_.setOrigin(0, 2.5);
		line_.setRotation(angle);
		line_.setPosition(start->pos());
		line_.setFillColor(sf::Color(200, 200, 200));

#ifdef _DEBUG
		std::cout << std::format("adding connection at ({},{}) with angle {} and length {}\n", start->pos().x, start->pos().y, angle, distance);
#endif
	}
}
