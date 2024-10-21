#include "Connection.hpp"
#include <format>
#include <iostream>

#include "Node.hpp"

namespace astar
{
	Connection::Connection(const Node* start, const Node* end, unsigned int cost, const float distance, const float angle) : start_{ start }, end_{ end }, cost_{ cost }
	{
		line_.setSize({ distance, 5 });
		line_.setOrigin(0, 2.5);
		line_.setRotation(angle);
		line_.setPosition(start->getPos());

		std::cout << std::format("adding connection at ({},{}) with angle {} and length {}\n", start->getPos().x, start->getPos().y, angle, distance);
	}

	void Connection::draw(sf::RenderTarget& rt) const
	{
		rt.draw(line_);
	}
}
