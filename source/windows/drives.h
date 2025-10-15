#pragma once
#include <vector>
#include <string>
#include "platform.h"

enum class Drive_type
{
	UNKNOWN,
	HARDDRIVE,
	ROM,
	REMOVABLE,
	NETWORK
};

std::string drivetype_to_string(Drive_type dt);

struct Drive_item {
	const stdstring name;
	const stdstring description;
	const uint64_t size;
	const Drive_type type;
	const bool is_mounted;
};

std::vector<Drive_item> get_drives();