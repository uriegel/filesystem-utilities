#include <Windows.h>
#include <vector>
#include <string>
#include "../files.h"
#include "../error.h"
using namespace std;

uint64_t convert_windowstime_to_unixtime(const FILETIME& ft) {
	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	return (ull.QuadPart / 10000000ULL - 11644473600ULL) * 1000;
}

tuple<int, wstring, wstring, vector<File_item>> get_files(const wstring& directory, bool show_hidden) {
    auto search_string = (directory[directory.length()-1] == L'\\' || directory[directory.length()-1] == L'/') 
        ? directory + L"*.*"s
        : directory + L"\\*.*"s;
    replace(search_string.begin(), search_string.end(), L'/', L'\\'); 

	vector<File_item> items;
    WIN32_FIND_DATAW w32fd{ 0 };
    auto ret = FindFirstFileW(search_string.c_str(), &w32fd);
    if (ret == INVALID_HANDLE_VALUE) {
        auto err = GetLastError();
        return tuple_cat(make_result(err), make_tuple(move(items)));
    }

    while (FindNextFileW(ret, &w32fd) == TRUE) {
        auto isHidden = ((w32fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN) || w32fd.cFileName[0] == L'.';
        if ((!isHidden || show_hidden) && (w32fd.cFileName[0] != L'.' || w32fd.cFileName[1] != L'.'))
            items.emplace_back(
                w32fd.cFileName,
                (w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY,
                isHidden,
                static_cast<uint64_t>(w32fd.nFileSizeHigh) << 32 | w32fd.nFileSizeLow,
                convert_windowstime_to_unixtime(w32fd.ftLastWriteTime)
        );
    }
	FindClose(ret);
    tuple<int, wstring, wstring, vector<File_item>> result(0, L""s, L""s, move(items));
	return result;
}
