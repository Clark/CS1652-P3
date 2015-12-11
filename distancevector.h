#ifndef _distancevector
#define _distancevector

#include "node.h"

class DistanceVector: public Node {
    private:
        // Anything you need in addition to Node members
				// Get min cost to node y
				double Min(const int y);
				// Recompute node's distance vector, returns true if a distance was changed
				bool RecomputeVector();
				bool IsNeighbor(const int node_number);
				void sendVectorToNeighbors();
        int GetNextHopN(int destination); // GetNextHop using node numbers only
				const static double INFINITE= 9999;
    public:
        DistanceVector(unsigned, SimulationContext* , double, double);
        DistanceVector(const DistanceVector &);
        DistanceVector & operator=(const DistanceVector &);
        ~DistanceVector();
        // Inherited from Node
        void LinkHasBeenUpdated(Link *l);
        void ProcessIncomingRoutingMessage(RoutingMessage *m);
        void TimeOut();
        Node* GetNextHop(Node* destination);
        Table* GetRoutingTable();
        ostream & Print(ostream & os) const;

        // Anything else
};

inline ostream & operator<<(ostream & os, const DistanceVector & n) {
    return n.Print(os);
}

#endif
