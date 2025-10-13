#pragma once
#include <string>
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include "linux/platform.h"
#endif
#include "get_exif_infos_worker.h"

uint64_t get_exif_date(const stdstring& file);
std::vector<ExifInfo> get_exif_infos(std::vector<ExifInfosInput>& input, stdstring cancellation);