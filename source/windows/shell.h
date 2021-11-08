#pragma once
#include <napi.h>
#include <string>
#include <vector>
#include "../FileResult.h"

void show_properties(const wchar_t* path);
void open(const wchar_t* path);
void open_as(const wchar_t* path);
void create_directory(const std::wstring& path, std::string& error, int& error_code);
void rename(std::wstring name, std::wstring new_name, std::string& error, int& error_code);
void delete_files(const std::vector<std::wstring>& files, std::wstring& error_description, FileResult& error_code);
void copy_files(std::wstring source_path, std::wstring target_path, const std::vector<std::wstring>& files, 
        const std::vector<std::wstring>& exceptions, bool move, std::string& error, int& error_code);
