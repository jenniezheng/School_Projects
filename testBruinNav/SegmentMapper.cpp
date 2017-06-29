#include "provided.h"
#include "MyMap.h"
#include <vector>
#include "support.h"
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
    MyMap<GeoCoord,vector<StreetSegment>> m_coordinates;
    void addSegment(const GeoCoord& cord, const StreetSegment& streetSeg);
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    for(unsigned i=0; i<ml.getNumSegments(); i++){
        StreetSegment streetSegment;
        ml.getSegment(i,streetSegment);
        addSegment(streetSegment.segment.start, streetSegment);
        addSegment(streetSegment.segment.end, streetSegment);
        for(unsigned i=0; i<streetSegment.attractions.size(); i++){
            Attraction a=streetSegment.attractions[i];
            addSegment(a.geocoordinates, streetSegment);
        }
    }
    if(printLevel>=1) {
        cerr<<"Number of Coordinates "<<m_coordinates.size()<<endl;
        cerr<<"Number of Segments "<<ml.getNumSegments()<<endl;
    }
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    return *(m_coordinates.find(gc));
}

void SegmentMapperImpl::addSegment(const GeoCoord& cord, const StreetSegment& streetSeg){
    vector<StreetSegment>* vecSegs=m_coordinates.find(cord);
    if(vecSegs!=nullptr)
        vecSegs->push_back(streetSeg);//vector exists, just add to vector
    else {
        vector<StreetSegment> vecSegs= vector<StreetSegment>();//create new vector and associate it
        vecSegs.push_back(streetSeg);
        m_coordinates.associate(cord,vecSegs);
    }
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
