#pragma once
#include <tuple>
#if WINDOWS
#include "windows/platform.h"
#elif LINUX
#include "linux/platform.h"
#endif

std::tuple<int, stdstring, stdstring> delete_files(const std::vector<stdstring>& files);