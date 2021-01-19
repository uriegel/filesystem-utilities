#include <windows.h>
#include <algorithm>
#include <array>
#include "utils.h"
#include "..\std_utils.h"
using namespace std;

string format_message(int last_error) {
    char* message{nullptr};
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    nullptr, last_error, 0, reinterpret_cast<char*>(&message), 0, nullptr);
    string result(message);
    LocalFree(message);
    return result;
}

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