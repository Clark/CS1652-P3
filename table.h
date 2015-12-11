#ifndef _table
#define _table

#include <iostream>
#include <map>
#include <vector>

#include "link.h"
//#include "linkstate.h"
//#include "node.h"

using namespace std;

struct TopoLink {
    TopoLink(): cost(-1), age(0) {}

    TopoLink(const TopoLink & rhs) {
        *this = rhs;
    }

    TopoLink & operator=(const TopoLink & rhs) {
        this->cost = rhs.cost;
        this->age = rhs.age;

        return *this;
    }

    int cost;
    int age;
};

// Students should write this class
class Table {
    private:
        //map < int, map < int, TopoLink > > topo;
    public:
        map < int, map < int, TopoLink > > topo; // FIX IMPORTANT
        Table();
        Table(const Table &);
        Table & operator=(const Table &);

        ostream & Print(ostream &os) const;

        // Anything else you need

        #if defined(LINKSTATE)
            int get_next_hop(unsigned dest);
            Table *get_routing_table() const;

            // It should also be updatable, as its contents will change with link updates and routing messages
            int update(const Link *l);

            // link state requires the use of dijkstra's algorithm
            void dijkstra();

            map<int, int> hopTo;
        #endif

        #if defined(DISTANCEVECTOR)
				double GetLatency(const int x, const int y);
				map<int, double> GetLatenciesFrom(const int x); // Return node x's distance vector
				void UpdateLatency(const int x, const int y, const double l);
        #endif
};

inline ostream & operator<<(ostream &os, const Table & t) { return t.Print(os);}


#endif
