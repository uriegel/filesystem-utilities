#pragma once
#include <optional>
#include "platform.h"

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