#pragma once
#include <vector>
#include <string>
#include "platform.h"

enum class Drive_type {
	UNKNOWN,
	HARDDRIVE,
	ROM,
	REMOVABLE,
	NETWORK,
	HOME
};

std::string drivetype_to_string(Drive_type dt);

struct Drive_item {
	stdstring name;
	stdstring description;
	uint64_t size;
	Drive_type type;
	bool is_mounted;
};

std::vector<Drive_item> get_drives();