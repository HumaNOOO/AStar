#pragma once

namespace astar 
{
	class Node;
	class Connection
	{
	public:
		const Node* end_;
		unsigned int cost_;
		bool render_;
		Connection(const Node* const end, unsigned int cost, bool render);
	};
}