#pragma once
#include <napi.h>
#include <string>
#include <vector>

void show_properties(const wchar_t* path);
void open(const wchar_t* path);
void open_as(const wchar_t* path);
void create_directory(const std::wstring& path, std::wstring& error, int& error_code);
void rename(std::wstring name, std::wstring new_name, std::string& error, int& error_code);
std::tuple<int, std::wstring, std::wstring> delete_files(const std::vector<std::wstring> &files);
void copy_files(const std::vector<std::wstring>& source_pathes, const std::vector<std::wstring>& target_pathes, 
        bool overwrite, bool move, std::string& error, int& error_code);