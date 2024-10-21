#pragma once

#include <SFML/Graphics.hpp>

namespace astar 
{
	class Node;
	struct Connection
	{
		sf::RectangleShape line_;
		Node* end_;
		Node* start_;
		unsigned int cost_;
		Connection(Node* start, Node* end, unsigned int cost, const float distance, const float angle);
	};
}
