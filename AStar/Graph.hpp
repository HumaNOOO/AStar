#pragma once

#include <vector>
#include "Node.hpp"


namespace astar
{
	class Graph
	{
	public:
		void operator=(const Graph&) = delete;
		Graph(const Graph&) = delete;

		static Graph& getInstance();
		void addNode(const float x, const float y);
		void addNode(const sf::Vector2f& pos);
		void draw(sf::RenderTarget& rt, const sf::Vector2f& mousePos) const;
		void checkAndDelete(const sf::Vector2f& mousePos);
		void setDrawDistance(const bool shouldDraw);
	private:
		Graph();
		std::vector<astar::Node> nodes_;
		sf::Font font_;
		bool drawDistance_;
	};
}
