#include "Connection.hpp"


namespace astar
{
	Connection::Connection(Node* end, unsigned int cost, bool render) :end_{ end }, cost_{ cost }, render_{render}
	{
	}
}
