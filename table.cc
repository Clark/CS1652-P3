#include "table.h"

Table::Table() {
    topo.clear();
}

Table::Table(const Table & rhs) {
    *this = rhs;
}

Table & Table::operator=(const Table & rhs) {
    /* For now,  Change if you add more data members to the class */
    topo = rhs.topo;

    return *this;
}

#if defined(GENERIC)
ostream & Table::Print(ostream &os) const
{
  os << "Generic Table()";
  return os;
}
#endif

#if defined(LINKSTATE)
ostream & Table::Print(ostream &os) const
{
  os << "LinkState Table()";
  return os;
}

int Table::update(const Link *l) {
  topo[l -> GetSrc()][l -> GetDest()].cost = l -> GetLatency();
  return topo[l -> GetSrc()][l -> GetDest()].age++;
}

#endif

#if defined(DISTANCEVECTOR)
map<int, map<int, double> > latencytable; // All distance vectors

ostream & Table::Print(ostream &os) const
{
	// FORMAT: Source -> [Destination, Latency]
	os << "\n";
	map<int, map<int, double> >::iterator x_it;
	map<int, double>::iterator y_it;
	for (x_it = latencytable.begin(); x_it != latencytable.end(); x_it++) 
	{
		int x = x_it->first;
		os << x << "->";
		map<int, double> x_vector = x_it->second;
		for (y_it = x_vector.begin(); y_it != x_vector.end(); y_it++)
		{
			int y = y_it->first;
			int l = y_it->second;
			os << " [" << y << "," << l << "]";
		}
		os << "\n";
	}	
  return os;
}

map<int, double> Table::GetLatenciesFrom(const int x) 
{
	map<int, double> copy;
	copy.insert(latencytable[x].begin(), latencytable[x].end());
	return copy;
}

double Table::GetLatency(const int x, const int y)
{
	return latencytable[x][y];
}

void Table::UpdateLatency(const int x, const int y, const double l)
{
	latencytable[x][y] = l;
}
#endif
