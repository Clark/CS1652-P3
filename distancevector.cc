#include "distancevector.h"
#include <math.h>

DistanceVector::DistanceVector(unsigned n, SimulationContext* c, double b, double l) :
    Node(n, c, b, l)		
{}

DistanceVector::DistanceVector(const DistanceVector & rhs) :
    Node(rhs)
{
    *this = rhs;
}

DistanceVector & DistanceVector::operator=(const DistanceVector & rhs) {
    Node::operator=(rhs);
    return *this;
}

DistanceVector::~DistanceVector() {}

int DistanceVector::GetNextHopN(int destination) {
		int x = number; 														// curr node
		int y = destination; 												// dest node
		int next_hop = x; 													// next node to hop to
		double next_cost = INFINITE;								// total cost to dest through next hop
		deque<Node*> * neighbors = GetNeighbors();
		// Run Bellman-Ford on neighbors to get min cost hop
		while (!neighbors->empty())
		{
			int v = neighbors->front()->GetNumber();
			int cost_xv = routing_table.GetLatency(x, v);
			int cost_vy = routing_table.GetLatency(v, y);
			int cost_xvy = cost_xv + cost_vy; 				// Cost to dest through neighbor
			if (cost_xvy < next_cost) 
			{
				// Replace with better hop
				next_hop = neighbors->front()->GetNumber();
				next_cost = cost_xvy;
			} 
			neighbors->pop_front();
		}
    return next_hop;
}

void DistanceVector::sendVectorToNeighbors() {
	int self = number;
	deque<Node*> * neighbors = GetNeighbors();
	// Send dist vector to all neighbors w
	while (!neighbors->empty()) {
		int w = neighbors->front()->GetNumber();
		map<int, double> self_dist_vector = routing_table.GetLatenciesFrom(self);
		// Iterate through dist vector and check if poisoned reverse is needed
		map<int, double>::iterator it;
		for (it = self_dist_vector.begin(); it != self_dist_vector.end(); it++) 
		{
			int y = it->first;
			if (y == self || y == w) continue;
			int next = GetNextHopN(y);
			// If self routes through w to get to y, add poisoned reverse.
			if (next == w) {
				self_dist_vector[y] = INFINITE;
				continue;
			}
		}
		// Send vector to neighbor
		SendToNeighbor(neighbors->front(), new RoutingMessage(self, self_dist_vector));
		neighbors->pop_front();
	}	
}

bool DistanceVector::IsNeighbor(const int node_number) {
	deque<Node*> * neighbors = GetNeighbors();
	while(!neighbors->empty()) 
	{
		int n = neighbors->front()->GetNumber();
		if (node_number == n) return true;
		neighbors->pop_front();
	}
	return false;
}


// Get the minimum distance from self to node y
double DistanceVector::Min(const int y) {
	int x = number;
	if (x == y) return 0;
	double min_xy = routing_table.GetLatency(x,y);
	deque<Node*> * neighbors = GetNeighbors();
	// Get cost to y through all neighbors
	// Keep minimum distance
	while (!neighbors->empty()) {
		int v = neighbors->front()->GetNumber();
		double cost_xv = routing_table.GetLatency(x, v);
		double cost_vy = routing_table.GetLatency(v, y);
		double cost_xvy = cost_xv + cost_vy;
		if (cost_xvy < min_xy) min_xy = cost_xvy;
		neighbors->pop_front();
	}
	if (min_xy == 0) return INFINITE; // If min is 0, it is unknown. Return infinity.
	return min_xy;
}

// Recompute distance vector for self.
// If any distance was changed, return true;
bool DistanceVector::RecomputeVector() {
	bool dist_changed = false;
	int x = number;
	// Retrieve costs to neighbors
	deque<Link*> * links = GetOutgoingLinks();
	while (!links->empty()) {
		int w = links->front()->GetDest();
		int old_cost = routing_table.GetLatency(x, w);
		int new_cost = links->front()->GetLatency();
		if (old_cost != new_cost) {
			routing_table.UpdateLatency(x, w, new_cost);
			dist_changed = true;
		}
		links->pop_front();
	}
	// Run Bellman-Ford over x's non-neighbor destinations
	map<int, double> dist_vector = routing_table.GetLatenciesFrom(x);
	map<int, double>::iterator it;
	for (it = dist_vector.begin(); it != dist_vector.end(); it++) 
	{
		int y = it->first;
		if (x == y || IsNeighbor(y)) continue;
		double old_dist = it->second;
		double new_dist = Min(y);
		// Update new distance
 		if (old_dist != new_dist) {
			routing_table.UpdateLatency(x, y, new_dist);
			dist_changed = true;
		} 
	}
	return dist_changed;
}


/** Write the following functions.  They currently have dummy implementations **/
void DistanceVector::LinkHasBeenUpdated(Link* l) {
    cerr << *this << ": Link Update: " << *l << endl;
		// Update latency according to link
		int x = l->GetSrc();		// Src
		int y = l->GetDest();		// Dest
		double latency = l->GetLatency();
		routing_table.UpdateLatency(x, y, latency);
		// Recompute own dist vector
		bool dist_changed = RecomputeVector();
		cerr << routing_table << endl;
		// Send own dist vector to neighbors if change detected
		if (dist_changed)
		{
			sendVectorToNeighbors();
		}
}

void DistanceVector::ProcessIncomingRoutingMessage(RoutingMessage *m) {
    cerr << *this << " got a routing message: " << *m << endl;
		// Store new dist vector for node x (the message sender)
		int x = m->node_number;
		map<int, double>::iterator it;
		for (it = m->dist_vector.begin(); it != m->dist_vector.end(); it++) 
		{
			int y = it->first;
			double l = it->second;
			routing_table.UpdateLatency(x, y, l);	
		}
		// Recompute own dist vector
		bool dist_changed = RecomputeVector();
		cerr << routing_table << endl;
		// Send own dist vector to neighbors if change detected
		if (dist_changed)
		{
			sendVectorToNeighbors();
		}
}

void DistanceVector::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;
}

Node* DistanceVector::GetNextHop(Node *destination) {
		int x = number; 														// curr node
		int y = destination->GetNumber(); 					// dest node
		Node * next_hop; 														// next node to hop to
		double next_cost = INFINITE;								// total cost to dest through next hop
		deque<Node*> * neighbors = GetNeighbors();
		// Run Bellman-Ford on neighbors to get min cost hop
		while (!neighbors->empty())
		{
			int v = neighbors->front()->GetNumber();
			int cost_xv = routing_table.GetLatency(x, v);
			int cost_vy = routing_table.GetLatency(v, y);
			int cost_xvy = cost_xv + cost_vy; 				// Cost to dest through neighbor
			if (cost_xvy < next_cost) 
			{
				// Replace with better hop
				next_hop = neighbors->front();
				next_cost = cost_xvy;
			} 
			neighbors->pop_front();
		}
    return next_hop;
}

Table* DistanceVector::GetRoutingTable() {
    return &routing_table;
}

ostream & DistanceVector::Print(ostream &os) const { 
    Node::Print(os);
    return os;
}
