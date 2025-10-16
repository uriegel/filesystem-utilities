#pragma once
#include <string>
#include <vector>
#include <optional>

std::wstring combine_path(std::wstring path, const std::wstring& path_to_combine);
void start_elevated();
std::string convertToString(const std::wstring &wstr);
std::wstring convertToWString(const std::string &str);
