#pragma once
#include <vector>
#include "platform.h"

enum class Drive_type
{
	UNKNOWN,
	HARDDRIVE,
	ROM,
	REMOVABLE,
	NETWORK
};

constexpr std::array<wstring_view, 5> drive_type_names = {
    L"UNKNOWN", L"HARDDRIVE", L"ROM", L"REMOVABLE", L"NETWORK"
};

constexpr std::wstring_view to_string(Drive_type dt) {
    for (std::size_t i = 0; i < drive_type_names.size(); ++i)
        if (drive_type_names[i] == dt)
            return drive_type_names[i];
    return L"UNKNOWN";
}

struct Drive_item {
	const stdstring name;
	const stdstring description;
	const uint64_t size;
	const Drive_type type;
	const bool is_mounted;
};

std::vector<Drive_item> get_drives();