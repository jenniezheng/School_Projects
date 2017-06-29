#include <string>
#include <list>
#include <queue>
#include <vector>
#include <algorithm>
#include "provided.h"
#include "MyMap.h"
#include "support.h"
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
    //path point struct
    struct PathPoint{
        GeoCoord cord;
        PathPoint* parent;
        double distFromStart;
        double distFromEnd;
        double priority;
        PathPoint(GeoCoord cord, PathPoint* parent, double distToStart, double distToEnd){
            this->cord=cord;
            this->parent=parent;
            this->distFromStart=distToStart;
            this->distFromEnd=distToEnd;
            this->priority=distToStart+distToEnd;
        }
    };

    static bool comparePathPointers(const PathPoint* a, const PathPoint* b){
        return a->priority > b->priority;
    }

    //variables
    AttractionMapper m_attractionMapper;
    SegmentMapper m_segmentMapper;

    //checks whether attraction is among the streetsegments given
    bool targetIsAmongSegments(const vector<StreetSegment>& streetSegs, const GeoCoord& target) const;
    bool targetIsAmongSegment(const StreetSegment& streetSeg, const GeoCoord& target) const;

    //inserts in priority order
    void addPathPoint (MyMap<GeoCoord, double>& trackAlreadyVisiting,
            list<PathPoint*>& trackAllPathPoints,
            priority_queue<PathPoint*, vector<PathPoint*>, decltype(&comparePathPointers)>& toVisit,
            PathPoint* pathPoint) const{
        toVisit.push(pathPoint);
        trackAllPathPoints.push_back(pathPoint);
        trackAlreadyVisiting.associate(pathPoint->cord,pathPoint->priority);
    }

    //check whether on start and end on same segment and if so, returns true and updates directions
    bool startAndEndOnSameSegment(const GeoCoord& start, const GeoCoord& end, vector<NavSegment>& directions) const;

    //converts geocoords to nav segments
    vector<NavSegment> convertCordsToNavSegments(const vector<GeoCoord>& cords,
           const GeoCoord& start, const GeoCoord& end) const;

    //assuming start and end not on same segment, returns directions
    bool navigateTo(const GeoCoord& start, const GeoCoord& end, vector<NavSegment> &directions) const;

};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    MapLoader mapLoader;
    bool success=mapLoader.load(mapFile);
    if(!success)return false;
    m_attractionMapper.init(mapLoader);
    m_segmentMapper.init(mapLoader);
    return true;
}



bool NavigatorImpl::targetIsAmongSegments(const vector<StreetSegment>& streetSegs, const GeoCoord& target) const{
    for (unsigned i=0; i<streetSegs.size(); i++)
        if(targetIsAmongSegment(streetSegs[i],target)) return true;
    return false;
}

bool NavigatorImpl::targetIsAmongSegment(const StreetSegment& streetSeg, const GeoCoord& target) const{
    vector<Attraction> attracts=streetSeg.attractions;
    for(unsigned j=0; j<attracts.size(); j++)
      if(attracts[j].geocoordinates==target)
          return true;
    return false;
}

bool NavigatorImpl::startAndEndOnSameSegment(const GeoCoord& start, const GeoCoord& end, vector<NavSegment>& directions) const{
    StreetSegment startStreetSeg=m_segmentMapper.getSegments(start)[0];
    StreetSegment endStreetSeg=m_segmentMapper.getSegments(end)[0];
    if(startStreetSeg.segment.start==endStreetSeg.segment.start &&
            startStreetSeg.segment.end==endStreetSeg.segment.end) {//on same segment!
        GeoSegment seg(start, end);
        string proceedDir=directionOfLine(seg);
        NavSegment proceed(proceedDir, startStreetSeg.streetName, distanceEarthMiles(start,end), seg);
        directions.push_back(proceed);
        return true;
    }
    else return false;
}

bool NavigatorImpl::navigateTo(const GeoCoord& start, const GeoCoord& end,
                                           vector<NavSegment>& directions) const{
    MyMap<GeoCoord, double> trackAlreadyVisiting;
    list<PathPoint*> trackAllPathPoints;
    priority_queue<PathPoint*, vector<PathPoint*>, decltype(&comparePathPointers)> toVisit(&comparePathPointers);
    StreetSegment firstStreetSeg=m_segmentMapper.getSegments(start)[0];
    GeoCoord start1=firstStreetSeg.segment.start;
    GeoCoord start2=firstStreetSeg.segment.end;
    PathPoint* startPathPoint1=new PathPoint(start1, nullptr, distanceEarthMiles(start,start1), distanceEarthMiles(start1,end));
    PathPoint* startPathPoint2=new PathPoint(start2, nullptr, distanceEarthMiles(start,start2), distanceEarthMiles(start2,end));
    addPathPoint(trackAlreadyVisiting,trackAllPathPoints,toVisit, startPathPoint1);
    addPathPoint(trackAlreadyVisiting,trackAllPathPoints,toVisit, startPathPoint2);
    PathPoint* endPoint=nullptr;
    long long numVisited=2;

    while(!toVisit.empty()){
        PathPoint* current=toVisit.top();
        numVisited++;
        toVisit.pop();
        vector<StreetSegment> streetSegs=m_segmentMapper.getSegments(current->cord);

        //does this point contain the attraction?
        if(targetIsAmongSegments(streetSegs, end)){
            endPoint=current; break;
        }
        //which coordinates to check next?
        for (unsigned i=0; i<streetSegs.size(); i++) {
            GeoCoord nextCord;
            if(streetSegs[i].segment.start == current->cord) nextCord=streetSegs[i].segment.end;//locate connected cordinates
            else nextCord=streetSegs[i].segment.start;//locate connected cordinates
            double nextPointPriority= current->distFromStart+distanceEarthMiles(current->cord, nextCord)+distanceEarthMiles(end, nextCord);
            double* potentiallyBetterPriority=trackAlreadyVisiting.find(nextCord);

            if(potentiallyBetterPriority==nullptr || (*potentiallyBetterPriority) > nextPointPriority ){
                PathPoint* nextPoint=new PathPoint(nextCord, current,
                        current->distFromStart+distanceEarthMiles(current->cord, nextCord), distanceEarthMiles(end, nextCord));
                addPathPoint(trackAlreadyVisiting,trackAllPathPoints,toVisit, nextPoint);
            }
        }
    }

    vector<GeoCoord> cords;
   if(endPoint==nullptr) return false;

    cords.push_back(end);
    while(endPoint!=nullptr){
        cords.push_back(endPoint->cord);
        endPoint=endPoint->parent;//go backwards in links from end to start
    }
    cords.push_back(start);
    reverse(cords.begin(),cords.end());//go from front to back

   for(list<PathPoint*>::iterator it=trackAllPathPoints.begin(); it!=trackAllPathPoints.end(); it++)
       delete *it;  //delete all dynamic memory

    if(printLevel>=1){
        cerr<<"Coordinates in path: "<<cords.size()<<endl;
        cerr<<"PathPoints visited: "<<numVisited<<endl;
        cerr<<"PathPoints created: "<<trackAllPathPoints.size()<<endl;
        cerr<<"Coordinates created: "<<trackAlreadyVisiting.size()<<endl;
        if(printLevel>=3){
            cerr<<"Coordinates from beginning to end: "<<endl;
            for(unsigned i=0; i<cords.size(); i++)
                cerr<<cords[i]<<endl;
        }
    }
    directions=convertCordsToNavSegments(cords, start,end);
    return true;
}

vector<NavSegment> NavigatorImpl::convertCordsToNavSegments(const vector<GeoCoord>& cords,
                                                       const GeoCoord& start, const GeoCoord& end) const{
    vector<NavSegment> navSegs;
    double totalDistance=0;
    if(cords.size()==0 || start==end) return navSegs; //if no result or same start and end, no directions
    for(unsigned i=1; i<cords.size(); i++){
        vector<StreetSegment> streetSegs=m_segmentMapper.getSegments(cords[i]);
        string startStr, endStr;//start and end street segment names
        for(unsigned j=0; j<streetSegs.size(); j++){
            if( i==cords.size()-1 ||
                  (i==1 && targetIsAmongSegment(streetSegs[j], cords[i-1]) ) ||
                    ( streetSegs[j].segment.start == cords[i-1]) ||
                    ( streetSegs[j].segment.end == cords[i-1]))
                startStr=streetSegs[j].streetName;
            if( i==cords.size()-1 ||
                   ( i==cords.size()-2 && targetIsAmongSegment(streetSegs[j], cords[i+1]) )||
                    ( streetSegs[j].segment.start == cords[i+1])||
                    ( streetSegs[j].segment.end == cords[i+1]))
                endStr=streetSegs[j].streetName;
        }
        GeoSegment seg(cords[i-1], cords[i]);
        double distance=distanceEarthMiles(cords[i-1],cords[i]);
        totalDistance+=distance;
        string proceedDir=directionOfLine(seg);
        NavSegment proceed(proceedDir, startStr,distance, seg);
        navSegs.push_back(proceed);
        if(startStr!=endStr && i!=cords.size()-1) {
            string turnDir=directionOfTurn(seg, GeoSegment(cords[i], cords[i+1]));
            NavSegment turn(turnDir,endStr);
            navSegs.push_back(turn);
        }
    }
    if(printLevel>=1){
        cerr<<"Total distance: "<<totalDistance<<" miles"<<endl;
        cerr<<"Total nav segments: "<<navSegs.size()<<endl;
    }
    return navSegs;
}


NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    GeoCoord cordStart;
    bool found=m_attractionMapper.getGeoCoord(start,cordStart);
    if(!found) return NAV_BAD_SOURCE;
    GeoCoord cordEnd;
    found=m_attractionMapper.getGeoCoord(end,cordEnd);
    if(!found) return NAV_BAD_DESTINATION;
    bool onSameSeg=startAndEndOnSameSegment(cordStart,cordEnd,directions);
    if(onSameSeg)return NAV_SUCCESS;
    bool foundPath=navigateTo(cordStart,cordEnd,directions);
    if(foundPath)return NAV_SUCCESS;
    else return NAV_NO_ROUTE;
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
