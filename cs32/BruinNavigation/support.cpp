#include "support.h"
using namespace std;


//angle to string functions
string directionOfLine(const GeoSegment& seg) {
    double angle=angleOfLine(seg);
    if(0<=angle && angle<=22.5) return "east";
    else if(angle<=67.5) return "northeast";
    else if(angle<=112.5) return "north";
    else if(angle<=156.5) return "northwest";
    else if(angle<=202.5) return "west";
    else if(angle<=247.5) return "southwest";
    else if(angle<=292.5) return "south";
    else if(angle<=337.5) return "southeast";
    else return "east";
}

string directionOfTurn(const GeoSegment& seg1, const GeoSegment& seg2) {
    double angle=angleBetween2Lines(seg1,seg2);
    if(0<=angle && angle<=180) return "left";
    else return "right";
}

//GeoCoord comparefunctions
bool operator< (const GeoCoord& cord1, const GeoCoord& cord2){
    return cord1.latitude < cord2.latitude ||
               ( cord1.latitude==cord2.latitude && cord1.longitude < cord2.longitude );
}

bool operator==(const GeoCoord& cord1, const GeoCoord& cord2){
    return cord1.latitude==cord2.latitude && cord1.longitude==cord2.longitude;
}

//output functions
ostream& operator<<(ostream& os, const GeoCoord& coord){
    os<<coord.latitudeText+","+coord.longitudeText;
    return os;
}

ostream& operator<<(ostream& os, const StreetSegment& streetSeg){
    os<<"Street Seg: " + streetSeg.streetName+" ";
    os<<streetSeg.segment.start<<","<<streetSeg.segment.end;
    for(unsigned i=0; i<streetSeg.attractions.size(); i++)
        os<<"\n\t"+streetSeg.attractions[i].name<<"|"<<streetSeg.attractions[i].geocoordinates;
    os<<'\n';
    return os;
}

ostream& operator<<(ostream& os, const vector<NavSegment>& nav){
    if(printLevel>=3){
        os<<"\nUnabbreviated navigation directions: \n";
        for(unsigned i=0; i<nav.size(); i++)
            if(nav[i].m_command==NavSegment::NavCommand::PROCEED)
                os<<"Proceed "+to_string(nav[i].m_distance) +" miles "+nav[i].m_direction+" on "+nav[i].m_streetName + '\n';
            else os<<"Take a "+nav[i].m_direction+" turn on " + nav[i].m_streetName+'\n';
    }
    else{
    os<<"\nShortened navigation directions: \n";
        double distance=0;
        for(unsigned i=0; i<nav.size(); i++){
            if(nav[i].m_command==NavSegment::NavCommand::PROCEED){
                distance+=nav[i].m_distance;
                if(i==nav.size()-1 || nav[i+1].m_command==NavSegment::NavCommand::TURN){
                    os<<"Proceed "+to_string(distance) +" miles "+nav[i].m_direction+" on "+nav[i].m_streetName+'\n';
                    distance=0;
                }
            }
            else os<<"Take a "+nav[i].m_direction+" turn on " + nav[i].m_streetName<<endl;
        }
    }
    return os;
}



