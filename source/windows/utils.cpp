#include <windows.h>
#include <algorithm>
#include <array>
#include "utils.h"
#include "..\std_utils.h"
using namespace std;

wstring combine_path(wstring path, const wstring& path_to_combine) {
    if (path.length() > 0 && path[path.length() - 1] != L'\\')
        path.append(L"\\");
    path.append(path_to_combine);
    return move(path);
}

void start_elevated() {
	array<wchar_t, 1024> bytes;
	GetModuleFileNameW(nullptr, bytes.data(), bytes.size());
	ShellExecuteW(nullptr, L"runas", bytes.data(), nullptr, nullptr, SW_SHOW);
 }

 string convertToString(const wstring &wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, 
                                        wstr.c_str(), (int)wstr.size(), 
                                        nullptr, 0, nullptr, nullptr);
    string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, 
                        wstr.c_str(), (int)wstr.size(), 
                        &str[0], size_needed, nullptr, nullptr);
    return str;
}

wstring convertToWString(const string &str) {
    if (str.empty()) return {};
    int size_needed = MultiByteToWideChar(CP_UTF8, 0,
                                          str.c_str(), (int)str.size(),
                                          nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0,
                        str.c_str(), (int)str.size(),
                        &wstr[0], size_needed);
    return wstr;    
}