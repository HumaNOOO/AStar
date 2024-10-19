#pragma once

namespace astar 
{
	class Node;
	struct Connection
	{
		Node* end_;
		unsigned int cost_;
		bool render_;
		Connection(Node* end, unsigned int cost, bool render);
	};
}