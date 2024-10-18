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
		void draw(sf::RenderTarget& rt, const sf::Vector2f& mousePos);
		void checkAndDelete(const sf::Vector2f& mousePos);
		void toggleDrawDistance();
		void setCollision(const sf::Vector2f& mousePos);
		void makeConnection(const sf::Vector2f& mousePos);
		bool addIdConnection(const std::pair<int, int>& connection);
		void resetNodes();
		void deleteNode(const int id);
		void update();
		void increaseOffset(const float offset);
		Node* checkMouseOnSomething(const sf::Vector2f& mousePos);
		void moveNode(sf::Vector2f mousePos);
		void clearSavedNode();
		void setDrawIds(const bool drawIds);
		void selectNodes(const sf::Vector2f& mousePos);
		bool isBuildConnectionMode() const;
		void toggleConnectionMode();
		const std::vector<std::pair<int, int>>& getConnectionsCRef() const;
	private:
		bool nodeWithIdExists(const int id) const;
		bool connectionExists(const std::pair<int, int>& connection) const;
		void handleRecalculate();
		float getAngleDeg(const sf::Vector2f p1, const sf::Vector2f p2) const;
		float offset_;
		bool drawIds_;
		Graph();
		Node* savedNode_; //Saved Node to make Connections between
		std::vector<Node> nodesCached_;
		sf::Font font_;
		sf::Text connectionText_;
		sf::Text text_;
		bool drawDistance_;
		bool nodesChanged_;
		long freeInd_;
		bool shouldRecalculate_;
		bool buildConnectionMode_;
		Node* startTarget_;
		Node* endTarget_;
		std::vector<std::pair<int, int>> connections_;
	};
}
