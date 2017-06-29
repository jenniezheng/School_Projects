#ifndef SUPPORT_H
#define SUPPORT_H

#include <string>
#include <iostream>
#include "provided.h"
using namespace std;

//geocoord functions
bool operator<(const GeoCoord& cord1, const GeoCoord& cord2);
bool operator==(const GeoCoord& cord1, const GeoCoord& cord2);

//angle to string functions
string directionOfLine(const GeoSegment& seg);
string directionOfTurn(const GeoSegment& seg1, const GeoSegment& seg2);

//for cerr
const unsigned printLevel=0;//0 = no printing to stderr, 3 = in depth printing
ostream& operator<<(ostream& os, const GeoCoord& cord);
ostream& operator<<(ostream& os, const StreetSegment& streetSeg);
ostream& operator<<(ostream& os, const vector<NavSegment>& nav);

#endif // SUPPORT_H
