#pragma once
#include <napi.h>
#include <string>
#include <vector>

void show_properties(const wchar_t* path);
void open(const wchar_t* path);
void open_as(const wchar_t* path);
std::tuple<int, std::wstring, std::wstring> create_directory(const std::wstring& path);
std::tuple<int, std::wstring, std::wstring> rename(std::wstring name, std::wstring new_name);
std::tuple<int, std::wstring, std::wstring> delete_files(const std::vector<std::wstring> &files);
std::tuple<int, std::wstring, std::wstring> copy_files(const std::vector<std::wstring> &source_pathes, const std::vector<std::wstring> &target_pathes,
                                                       bool overwrite, bool move);
std::tuple<int, std::wstring, std::wstring> rename(std::wstring name, std::wstring new_name);
