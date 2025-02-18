#ifndef TSP_H
#define TSP_H

#include <vector>

// The potential routes the saleperson could take
typedef std::vector< std::vector<int> > MAP;

std::vector<int> SolveTSP(char const* filename);
#endif
