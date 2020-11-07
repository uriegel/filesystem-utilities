#pragma once
#include <string>
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include "linux/platform.h"
#endif

uint64_t get_exif_date(const stdstring& file);