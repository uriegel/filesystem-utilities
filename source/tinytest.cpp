#include "tinyxml2.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring> // for strchr

using namespace tinyxml2;

struct GpxPoint {
    double lat = 0.0;
    double lon = 0.0;
    double ele = 0.0;
    std::string time;
};

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

std::vector<GpxPoint> loadGpxTrack(const std::string& path) {
    std::vector<GpxPoint> track;
    XMLDocument doc;

    if (doc.LoadFile(path.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load GPX file: " << path << std::endl;
        return track;
    }

    XMLElement* gpx = doc.RootElement();
    if (!gpx) return track;

    for (XMLElement* trk = FirstChildElementNS(gpx, "trk"); trk; trk = NextSiblingElementNS(trk, "trk")) {
        for (XMLElement* trkseg = FirstChildElementNS(trk, "trkseg"); trkseg; trkseg = NextSiblingElementNS(trkseg, "trkseg")) {
            for (XMLElement* trkpt = FirstChildElementNS(trkseg, "trkpt"); trkpt; trkpt = NextSiblingElementNS(trkpt, "trkpt")) {
                GpxPoint point{};

                // Read lat/lon attributes
                trkpt->QueryDoubleAttribute("lat", &point.lat);
                trkpt->QueryDoubleAttribute("lon", &point.lon);

                // Optional: read elevation
                if (XMLElement* ele = FirstChildElementNS(trkpt, "ele"))
                    ele->QueryDoubleText(&point.ele);

                // Optional: read timestamp
                if (XMLElement* time = FirstChildElementNS(trkpt, "time"))
                    point.time = time->GetText() ? time->GetText() : "";

                track.push_back(point);
            }
        }
    }

    return track;
}

void maintest() {
    auto track = loadGpxTrack("/media/uwe/Daten/Bilder/Fotos/2024/Tracks/2024-03-10-12-44.gpx");
    std::cout << "Loaded " << track.size() << " points.\n";
    if (!track.empty()) {
        std::cout << "First: lat=" << track[0].lat
                  << ", lon=" << track[0].lon
                  << ", ele=" << track[0].ele
                  << ", time=" << track[0].time << "\n";
    }
}
