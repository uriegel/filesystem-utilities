#pragma once
#include <sstream>
#include <vector>
// #include <locale>
// #include <codecvt>

const std::vector<std::wstring> split(const std::wstring& s, wchar_t delim);
std::wstring join(const std::vector<std::wstring>& arr, wchar_t delim);
int findString(const std::wstring &text, const std::wstring& searchText);

// inline std::string ws2utf8(const std::wstring &input) {
// 	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
// 	return utf8conv.to_bytes(input);
// }

// inline std::wstring utf82ws(const std::string &input) {
// 	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
// 	return utf8conv.from_bytes(input);
// }

bool ends_with(std::wstring const& value, std::wstring const& ending);