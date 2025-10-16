#pragma once
#include <vector>
#if WINDOWS
#include "windows/platform.h"
#elif LINUX
#include "linux/platform.h"
#endif

struct File_item {
	File_item(stdstring&& name, bool is_directory,  bool is_hidden, uint64_t size, uint64_t time)		
	: name(name)
	, is_directory(is_directory)
	, is_hidden(is_hidden)
	, size(size)
	, time(time) {}
	const stdstring name;
	const bool is_directory;
	const bool is_hidden;
	const uint64_t size;
	const uint64_t time;
};

#include <iostream>
#include <tuple>

std::tuple<int, stdstring, stdstring, std::vector<File_item>> get_files(const stdstring& directory, bool show_hidden);