#pragma once
#include <optional>
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include "linux/platform.h"
#endif

struct Version_info {
    Version_info(int major, int minor, int build, int patch) 
    : major(major)
    , minor(minor)
    , build(build)
    , patch(patch) {}

    int major;
    int minor;
    int build;
    int patch;
};

std::optional<Version_info> get_file_info_version(const stdstring& file_name);