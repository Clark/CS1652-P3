#include "messages.h"

RoutingMessage::RoutingMessage()
{}

RoutingMessage::RoutingMessage(const RoutingMessage &rhs) {
    *this = rhs; 	
}

RoutingMessage & RoutingMessage::operator=(const RoutingMessage & rhs) {
    /* For now.  Change if you add data members to the struct */

    return *this;
}

#if defined(GENERIC)
ostream &RoutingMessage::Print(ostream &os) const
{
    os << "Generic RoutingMessage()";
    return os;
}
#endif

#if defined(LINKSTATE)
RoutingMessage::RoutingMessage(int age, unsigned src, unsigned dest, double lat)
{
    age = age;
	source = src;
	destination = dest;
	latency = lat;
}

ostream &RoutingMessage::Print(ostream &os) const
{
    os << "LinkState RoutingMessage()";
    return os;
}
#endif

#if defined(DISTANCEVECTOR)
RoutingMessage::RoutingMessage(const int node_number, map<int, double> dist_vector) {
		this->node_number = node_number;
		this->dist_vector = dist_vector;
}

ostream &RoutingMessage::Print(ostream &os) const
{
    // os << "DistanceVector RoutingMessage(node " << node_number << ")";
		os << "DV-RM(sender=" << node_number << ")";
    return os;
}
#endif