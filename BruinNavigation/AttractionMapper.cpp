#include "provided.h"
#include "MyMap.h"
#include "support.h"
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
    bool getGeoCoord(string attraction, GeoCoord& gc) const;//stores attraction in lowercase
private:
    MyMap<string, GeoCoord> m_attractions;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
    for(unsigned i=0; i<ml.getNumSegments(); i++){
        StreetSegment streetSegment;
        ml.getSegment(i,streetSegment);
        for(unsigned j=0; j<streetSegment.attractions.size(); j++){
            Attraction a=streetSegment.attractions[j];
            string name=a.name;
            for(unsigned i=0; i<name.size(); i++)
                name[i]=tolower(name[i]);//convert to lower case before storing
            m_attractions.associate(name,a.geocoordinates);
        }
    }
    if(printLevel>=1)cerr<<"Number of Attractions "<<m_attractions.size()<<endl;
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    for(unsigned i=0; i<attraction.size(); i++)
        attraction[i]=tolower(attraction[i]);//convert to lowercase before searching
    const GeoCoord* cord=m_attractions.find(attraction);
    if(cord==nullptr)return false;
    else {gc=*cord; return true;}
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
