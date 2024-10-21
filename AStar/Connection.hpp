#pragma once

#include <SFML/Graphics.hpp>

namespace astar 
{
	class Node;
	struct Connection
	{
		sf::RectangleShape line_;
		const Node* end_;
		const Node* start_;
		unsigned int cost_;
		Connection(const Node* start, const Node* end, unsigned int cost, const float distance, const float angle);
		void draw(sf::RenderTarget& rt) const;
	};
}
