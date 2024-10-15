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
		void draw(sf::RenderTarget& rt, const sf::Vector2f& mousePos, bool makeConnection) const;
		void checkAndDelete(const sf::Vector2f& mousePos);
		void setDrawDistance(const bool shouldDraw);
		void setCollision(const sf::Vector2f& mousePos);
		void makeConnection(sf::Vector2f& mousePos);
		void resetNodes();
		void update();
		void increaseOffset(const float offset);
		Node* checkMouseOnSomething(sf::Vector2f mousePos);
		void moveNode(sf::Vector2f mousePos);
		void clearSavedNode();
		void setDrawIds(const bool drawIds);
		void selectNodes(const sf::Vector2f& mousePos);
	private:
		void handleRecalculate();
		float offset_;
		bool drawIds_;
		Graph();
		Node* savedNode_; //Saved Node to make Connections between
		std::vector<Node> nodesCached_;
		sf::Font font_;
		bool drawDistance_;
		bool nodesChanged_;
		long freeInd_;
		bool shouldRecalculate_;
		Node* startTarget_;
		Node* endTarget_;
	};
}
