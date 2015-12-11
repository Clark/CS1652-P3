#include "linkstate.h"

#include <vector>
#include <map>
#include "node.h"

LinkState::LinkState(unsigned n, SimulationContext* c, double b, double l) :
    Node(n, c, b, l)
{
    this->myTable = new Table();
}

LinkState::LinkState(const LinkState & rhs) :
    Node(rhs)
{
    *this = rhs;
}

LinkState & LinkState::operator=(const LinkState & rhs) {
    Node::operator=(rhs);
    return *this;
}

LinkState::~LinkState() {}


/** Write the following functions.  They numberly have dummy implementations **/
void LinkState::LinkHasBeenUpdated(Link* l) {
    cerr << *this << ": Link Update: " << *l << endl;
    int test = myTable->update(l);

    SendToNeighbors(new RoutingMessage(test, l->GetSrc(), l->GetDest(), l->GetLatency()));
}

void LinkState::ProcessIncomingRoutingMessage(RoutingMessage *m) {
    cerr << *this << " got a routing message: " << *m << " (ignored)" << endl;
    if(myTable -> topo[m -> source][m -> destination].age < (int) m -> age) {
        myTable -> topo[m -> source][m -> destination].cost = m -> latency;                                                                                                                                         
        myTable -> topo[m -> source][m -> destination].age = m -> age;
				deque<Node*> * neighbors = GetNeighbors();
				while(!neighbors->empty())
				{
					Node * n = neighbors->front();
					if (n->GetNumber() != m->source)
					{
						SendToNeighbor(n, m);
					}
					neighbors->pop_front();
				}
    }
    
}

void LinkState::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;
}

Node* LinkState::GetNextHop(Node *destination) {
    unsigned destinationNumber = destination->GetNumber();
    //cout << "test" << number << " to " << destination->GetNumber() << "\n";

    deque<Node*> *allNeighbors = this -> GetNeighbors();

    unsigned nodesInTopology = (myTable->topo).size();
    //cout << nodesInTopology << " NODES " << number << "\n";
    // STL structures for Dijkstra()
    // using just a large number to represent infinity distance
    // using just a large number to represent undefined parent
    vector<int> d(nodesInTopology, 1000000);    // DISTANCE
    vector<int> p(nodesInTopology, 1000000);    // PREVIOUS
    vector<bool> v(nodesInTopology, false);     // VISITED

    
    v[number] = true;   // already visited self
    d[number] = 0;      // distance to self is zero
    p[number] = NULL;   // first has no parent
    
    // INITIAL NODE:
    map<int, TopoLink> :: iterator topoIte;
    for(topoIte = myTable->topo[number].begin(); topoIte != myTable -> topo[number].end(); topoIte++) {
        d[topoIte -> first] = topoIte -> second.cost;
        p[topoIte -> first] = number;
        cout << "distance from " << number << " to " << topoIte -> first <<" is " << d[topoIte -> first] << "\n";
    }

    cout << "nodes in topology: " << nodesInTopology <<"     " << d.size() << "\n";
    // for all nodes in topology
    for(int i = 0; i < nodesInTopology - 1; i++) {
        int closest;
        int closestDistance = 1000000;

        // Finds the cnode with the smallest distance
        for(int j = 0; j < nodesInTopology; j++) {
            if(v[j] == false) {
                if(d[j] < closestDistance) {
                    closestDistance = d[j];
                    closest = j;
                }
            }
        }
        
        cout << " Visited neighbor: " << closest << " | distance: " << closestDistance <<"\n";
        v[closest] = true;
        unsigned result = 0;
        if(closestDistance != 1000000) result = closestDistance;

        // for each neighbor of closest, adjust distance
        map<int, TopoLink> :: iterator topoIte2;
        for(topoIte2 = myTable->topo[closest].begin(); topoIte2 != myTable->topo[closest].end(); topoIte2++) {
            // Only if not visited yet
            if(v[topoIte2 -> first] == false) {
                if((d[topoIte2 -> second.cost] + result) < d[topoIte2 -> first]) {
                    d[topoIte2 -> first] = d[topoIte2 -> second.cost] + result;
                    p[topoIte2 -> first] = closest;
                }            
            }
        }
    }

    int nodeNumber = destination -> GetNumber();
    int parentNode = p[destination -> GetNumber()];
    // Now that we've arrived at the destination
    // Iterate backwards to find the number of the node to hop to
    while(parentNode != number) {
        nodeNumber = parentNode;
        parentNode = p[nodeNumber];
    }
    cout << nodeNumber << " is returned " << "\n";
    // Get the actual node to return
    for(deque<Node*> :: iterator i = allNeighbors ->begin(); i != allNeighbors->end(); i++) {
        if((Node(nodeNumber, 0, 0, 0).Matches(*i))) {
            // return new Node(**i);
            return *i;
        }
    } 

    return NULL;
}

// called when the simulation wants to get a COPY of your number routing table
Table* LinkState::GetRoutingTable() {
    Table* copy = new Table(*myTable);
    return copy;
}

ostream & LinkState::Print(ostream &os) const { 
    Node::Print(os);
    return os;
}

void LinkState::Dijkstra() {

}
