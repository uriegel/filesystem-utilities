#pragma once
#include <vector>
#if WINDOWS
#include "windows/platform.h"
#elif LINUX
#include "linux/platform.h"
#endif

struct GpxPoint {
    double lat = 0.0;
    double lon = 0.0;
    double ele = 0.0;
    stdstring time;
};

struct GpxTrack {
    double distance = 0.0;
    int duration = 0;
    stdstring date;
    stdstring name;
    std::vector<GpxPoint> trackPoints{};
};

GpxTrack get_gpx_track(const stdstring& path);