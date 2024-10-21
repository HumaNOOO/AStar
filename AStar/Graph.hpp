#pragma once

#include <vector>
#include "Node.hpp"
#include <ranges>


namespace astar
{
	class Graph
	{
	public:
		void operator=(const Graph&) = delete;
		Graph(const Graph&) = delete;
		static Graph& get();
		void resetIndex();
		bool addNode(const sf::Vector2f& pos, const int id = -1, const bool collision = false);
		void draw(sf::RenderTarget& rt, const sf::Vector2f& mousePos);
		void checkAndDelete(const sf::Vector2f& mousePos);
		void toggleDrawDistance();
		void setCollision(const sf::Vector2f& mousePos);
		void makeConnection(const sf::Vector2f& mousePos);
		bool addIdConnection(const std::pair<int, int>& connection);
		void resetNodes();
		void deleteNode(const int id);
		void increaseOffset(const float offset);
		Node* checkMouseOnSomething(const sf::Vector2f& mousePos);
		void moveNode(sf::Vector2f mousePos);
		void clearSavedNode();
		void setDrawIds(const bool drawIds);
		void selectNodes(const sf::Vector2f& mousePos);
		bool isBuildConnectionMode() const;
		void toggleConnectionMode();
		bool setStart(const int id);
		bool setEnd(const int id);
		const std::vector<std::pair<int, int>>& getConnectionsCRef() const;
		const std::vector<Node>& getNodesCRef() const;
		void drawPath();
		void executeAStar();
		void toggleRapidConnect();
		bool isRapidConnect() const;
	private:
		bool nodeWithIdExists(const int id) const;
		bool connectionExists(const std::pair<int, int>& connection) const;
		void handleRecalculate();
		float offset_;
		bool drawIds_;
		Graph();
		Node* savedNode_; //Saved Node to make Connections between
		std::vector<Node> nodesCached_;
		std::vector<Connection> connectionsCached_;
		sf::Font font_;
		sf::Text connectionText_;
		sf::Text text_;
		bool drawDistance_;
		int freeInd_;
		bool shouldRecalculate_;
		bool buildConnectionMode_;
		bool rapidConnect_;
		Node* startTarget_;
		Node* endTarget_;
		std::vector<std::pair<int, int>> connections_;
	};
}
