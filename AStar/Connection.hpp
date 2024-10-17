#pragma once

namespace astar 
{
	class Node;
	class Connection
	{
	public:
		Node* end_;
		unsigned int cost_;
		bool render_;
		Connection(Node* end, unsigned int cost, bool render);
	};
}