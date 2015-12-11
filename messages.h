#ifndef _messages
#define _messages

#include <iostream>
#include "node.h"
#include "link.h"

struct RoutingMessage {
    RoutingMessage();
    RoutingMessage(const RoutingMessage &rhs);
    RoutingMessage &operator=(const RoutingMessage &rhs);

    ostream & Print(ostream &os) const;

    // Anything else you need

    #if defined(LINKSTATE)
    	RoutingMessage(int age, unsigned src, unsigned dest, double lat);
        int age;
    	unsigned source;
    	unsigned destination;
    	double latency;
    #endif

    #if defined(DISTANCEVECTOR)
		RoutingMessage(const int node_number, map<int, double> dist_vector);
		int node_number;
		map<int, double> dist_vector; // Cost from node to all other nodes
    #endif
};

inline ostream & operator<<(ostream &os, const RoutingMessage & m) { return m.Print(os);}

#endif
