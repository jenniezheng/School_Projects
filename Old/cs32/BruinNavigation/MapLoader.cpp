#include "provided.h"
#include <fstream>
#include "support.h"
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
    vector<StreetSegment> m_segments;
    bool addNextStreetSegment(istream& infile);
    GeoSegment getSegmentFromString( string geoCoordinatesStr);
    Attraction getAttractionFromString (string attractionStr);
};

MapLoaderImpl::MapLoaderImpl()
{
}

MapLoaderImpl::~MapLoaderImpl()
{
}

GeoSegment MapLoaderImpl::getSegmentFromString (string geoCoordinates){
    short cordStartPoints[4];
    short cordLengths[4];
    cordStartPoints[0]=0;
    cordStartPoints[1]=geoCoordinates.find(',', 1)+1;
    cordLengths[0]=cordStartPoints[1]-cordStartPoints[0]-1;
    while(geoCoordinates[cordStartPoints[1]]==' ') cordStartPoints[1]++;//skip space if it exists
    cordStartPoints[2]=geoCoordinates.find(' ', cordStartPoints[1]+1)+1;
    cordLengths[1]=cordStartPoints[2]-cordStartPoints[1]-1;
    cordStartPoints[3]=geoCoordinates.find(',', cordStartPoints[2]+1)+1;
    cordLengths[2]=cordStartPoints[3]-cordStartPoints[2]-1;
    while(geoCoordinates[cordStartPoints[3]]==' ') cordStartPoints[3]++;//skip space if it exists
    cordLengths[3]=geoCoordinates.size()-cordStartPoints[3];
    GeoCoord starting(geoCoordinates.substr(cordStartPoints[0], cordLengths[0]),
                      geoCoordinates.substr(cordStartPoints[1], cordLengths[1]));
    GeoCoord ending(geoCoordinates.substr(cordStartPoints[2], cordLengths[2]),
                    geoCoordinates.substr(cordStartPoints[3], cordLengths[3]));
    return GeoSegment(starting,ending);
}

Attraction MapLoaderImpl::getAttractionFromString (string attractionStr){
    Attraction a;
    short atCordStarts[2];
    short atCordLengths[2];
    atCordStarts[0]=attractionStr.find('|')+1;
    atCordStarts[1]=attractionStr.find(',',atCordStarts[0]+1)+1;
    atCordLengths[0]=atCordStarts[1]-atCordStarts[0]-1;
    while(attractionStr[atCordStarts[1]]==' ')atCordStarts[1]++;
    atCordLengths[1]=attractionStr.size()-atCordStarts[1];
    a.name=attractionStr.substr(0,attractionStr.find('|'));
    GeoCoord attractionCord(attractionStr.substr(atCordStarts[0], atCordLengths[0]),
                            attractionStr.substr(atCordStarts[1], atCordLengths[1]));
    a.geocoordinates=attractionCord;
    return a;
}

bool MapLoaderImpl::addNextStreetSegment(istream& infile)
{
    StreetSegment streetSegment;
    string streetSegmentName;
    if(! getline(infile, streetSegmentName)) return false;//no more segments
    streetSegment.streetName=streetSegmentName;
    string geoCoordinates;
    getline(infile, geoCoordinates);
    streetSegment.segment=getSegmentFromString(geoCoordinates);

    string numAttractions;
    getline(infile, numAttractions);
    int numAttracts=stoi(numAttractions);

    for(int i=0; i<numAttracts; i++){
        string attractionStr;
        getline(infile, attractionStr);
        streetSegment.attractions.push_back(getAttractionFromString(attractionStr));
    }
    m_segments.push_back(streetSegment);
    return true;
}

bool MapLoaderImpl::load(string mapFile)
{
    ifstream infile(mapFile);
    if ( ! infile ){ // Failed to open file
        cerr << "Error: Cannot open " << mapFile << endl;
        return false;
    }
    while (addNextStreetSegment(infile)) //while has more segments, add them
    { }
    return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
    return m_segments.size();
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    if( segNum>=getNumSegments())return false;
    else { seg=m_segments[segNum];return true; }
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
