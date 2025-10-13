#include "tinyxml2.h"
#include "gpx_track.h"
#include <vector>
using namespace std;
using namespace tinyxml2;

// Helper: strip XML namespace prefix (e.g. "gpx:trkpt" → "trkpt")
static const char* stripNamespace(const char* name) {
    if (!name) return "";
    const char* colon = std::strchr(name, ':');
    return colon ? colon + 1 : name;
}

// Helper: find first child element matching a local name (ignores namespaces)
static XMLElement* FirstChildElementNS(XMLElement* parent, const char* localName) {
    for (XMLElement* el = parent ? parent->FirstChildElement() : nullptr; el; el = el->NextSiblingElement()) {
        if (std::strcmp(stripNamespace(el->Name()), localName) == 0)
            return el;
    }
    return nullptr;
}

// Helper: find next sibling element matching a local name (ignores namespaces)
static XMLElement* NextSiblingElementNS(XMLElement* elem, const char* localName) {
    for (XMLElement* el = elem ? elem->NextSiblingElement() : nullptr; el; el = el->NextSiblingElement()) {
        if (std::strcmp(stripNamespace(el->Name()), localName) == 0)
            return el;
    }
    return nullptr;
}

GpxTrack get_gpx_track(const stdstring& path) {
    XMLDocument doc;
    GpxTrack gpxTrack{};
#ifdef LINUX    
    if (doc.LoadFile(path.c_str()) != XML_SUCCESS)
#else    
    if (doc.LoadFile("path.c_str()") != XML_SUCCESS)
#endif    
        return gpxTrack;

    XMLElement* gpx = doc.RootElement();
    if (!gpx) 
        return gpxTrack;

    for (XMLElement* trk = FirstChildElementNS(gpx, "trk"); trk; trk = NextSiblingElementNS(trk, "trk")) {
        for (XMLElement* info = FirstChildElementNS(trk, "info"); info; info = NextSiblingElementNS(info, "info")) {
            if (XMLElement* ele = FirstChildElementNS(info, "distance"))
                ele->QueryDoubleText(&gpxTrack.distance);
            if (XMLElement* ele = FirstChildElementNS(info, "duration"))
                ele->QueryIntText(&gpxTrack.duration);
#ifdef LINUX                    
            if (XMLElement* ele = FirstChildElementNS(info, "name"))
                gpxTrack.name = ele->GetText();
            if (XMLElement* ele = FirstChildElementNS(info, "date"))
                gpxTrack.date = ele->GetText();
#endif                    
        }
        for (XMLElement* trkseg = FirstChildElementNS(trk, "trkseg"); trkseg; trkseg = NextSiblingElementNS(trkseg, "trkseg")) {
            for (XMLElement* trkpt = FirstChildElementNS(trkseg, "trkpt"); trkpt; trkpt = NextSiblingElementNS(trkpt, "trkpt")) {
                GpxPoint point{};

                // Read lat/lon attributes
                trkpt->QueryDoubleAttribute("lat", &point.lat);
                trkpt->QueryDoubleAttribute("lon", &point.lon);

                // Optional: read elevation
                if (XMLElement* ele = FirstChildElementNS(trkpt, "ele"))
                    ele->QueryDoubleText(&point.ele);

#ifdef LINUX                                        
                // Optional: read timestamp
                if (XMLElement* time = FirstChildElementNS(trkpt, "time"))
                    point.time = time->GetText() ? time->GetText() : "";
#endif
                gpxTrack.trackPoints.push_back(point);
            }
        }
    }

    return gpxTrack;
}
