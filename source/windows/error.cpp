#include <windows.h>
#include "..\error.h"
using namespace std;

wstring format_message(int last_error) {
    wchar_t* message{nullptr};
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    nullptr, last_error, 0, reinterpret_cast<wchar_t*>(&message), 0, nullptr);
    wstring result(message);
    LocalFree(message);
    return result;
}

wstring format_error(int last_error) {
    switch (last_error) {
        case 3:
            return L"PATH_NOT_FOUND"s;
        case 5:
            return L"ACCESS_DENIED"s;
        default:
            return L"GENERAL"s;
    }
}
