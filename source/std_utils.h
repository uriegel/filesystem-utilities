#pragma once
#include <vector>
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include "linux/platform.h"
#endif
// #include <locale>
// #include <codecvt>

const std::vector<stdstring> split(const stdstring& s, stdchar delim);
stdstring join(const std::vector<stdstring>& arr, stdchar delim);
int findString(const stdstring &text, const stdstring& searchText);

// inline std::string ws2utf8(const std::wstring &input) {
// 	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
// 	return utf8conv.to_bytes(input);
// }

// inline std::wstring utf82ws(const std::string &input) {
// 	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
// 	return utf8conv.from_bytes(input);
// }

bool ends_with(stdstring const& value, stdstring const& ending);