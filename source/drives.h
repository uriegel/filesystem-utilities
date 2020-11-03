#pragma once
#include <vector>
#if WINDOWS
#include "windows/platform.h"
#elif LINUX
#include "linux/platform.h"
#endif

enum class Drive_type
{
	UNKNOWN,
	HARDDRIVE,
	ROM,
	REMOVABLE,
	NETWORK
};

struct Drive_item {
	const stdstring name;
	const stdstring description;
	const uint64_t size;
	const Drive_type type;
	const bool is_mounted;
};

std::vector<Drive_item> get_drives();