#pragma once

#include <vector>
#include "Node.hpp"
#include <ranges>
#include <optional>


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
		void addNodeForce(const sf::Vector2f pos, const int id = -1);
		void draw(const sf::Vector2f& mousePos);
		void drawStats();
		void checkAndDelete(const sf::Vector2f& mousePos);
		void toggleDrawDistance();
		void setCollision(const sf::Vector2f& mousePos);
		void makeConnection(const sf::Vector2f& mousePos);
		bool addIdConnection(const std::pair<int, int>& connection);
		void addIdConnectionForce(const int id1, const int id2);
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
		void toggleDrawScore();
		bool setStart(const int id);
		bool setEnd(const int id);
		const std::vector<std::pair<int, int>>& connectionsCRef() const;
		const std::vector<Connection>& connectionsCachedCRef() const;
		const std::vector<Node>& nodesCRef() const;
		std::optional<std::string> executeAStar();
		void toggleRapidConnect();
		bool isRapidConnect() const;
		void generateRandomGraph(const int nodesCount, const float chance, const float radius);
		bool isValidPosition(const sf::Vector2f pos, const float radius);
		void setRenderTarget(sf::RenderTarget* rt);
		void setAStarResult(const bool result, const float pathLength);
		float pathLength() const;
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
		bool drawScore_;
		bool pathFound_;
		float pathLength_;
		Node* startTarget_;
		Node* endTarget_;
		sf::RenderTarget* rt_;
		std::vector<std::pair<int, int>> connections_;
	};
}
