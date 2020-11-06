#include <Windows.h>
#include <vector>
#include <string>
#include "../files.h"
using namespace std;

uint64_t convert_windowstime_to_unixtime(const FILETIME& ft) {
	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	return (ull.QuadPart / 10000000ULL - 11644473600ULL) * 1000;
}

vector<File_item> get_files(const wstring& directory) {
    auto search_string = (directory[directory.length()-1] == L'\\' || directory[directory.length()-1] == L'/') 
        ? directory + L"*.*"s
        : directory + L"\\*.*"s;
    replace(search_string.begin(), search_string.end(), L'/', L'\\'); 

	vector<File_item> result;
    WIN32_FIND_DATAW w32fd{ 0 };
    auto ret = FindFirstFileW(search_string.c_str(), &w32fd);
    while (FindNextFileW(ret, &w32fd) == TRUE) {
        auto isHidden = (w32fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN
        if (!isHidden && (w32fd.cFileName[0] != L'.' || w32fd.cFileName[1] != L'.'))
            result.emplace_back(
                w32fd.cFileName,
                (w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY,
                isHidden,
                static_cast<uint64_t>(w32fd.nFileSizeHigh) << 32 | w32fd.nFileSizeLow,
                convert_windowstime_to_unixtime(w32fd.ftLastWriteTime)
        );
    }
	FindClose(ret);
	return result;
}
