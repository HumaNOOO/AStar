#include "Connection.hpp"
namespace astar {
	Connection::Connection(const Node* const end, unsigned int cost, bool render) :end_{ end }, cost_{ cost }, render_{render}
	{
		
	}
}