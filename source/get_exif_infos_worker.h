#pragma once
#include <napi.h>
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include "linux/platform.h"
#endif

struct ExifInfosInput {
    int idx;
    stdstring path;
};

struct ExifInfo {
    int idx;
    uint64_t date;
    double latitude;
    double longitude;
};

Napi::Value GetExifInfosAsync(const Napi::CallbackInfo& info);