#include <windows.h>
#include "..\error.h"
using namespace std;

wstring format_message(int last_error) {
    wchar_t* message{nullptr};
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    nullptr, last_error, 0, reinterpret_cast<wchar_t*>(&message), 0, nullptr);
    wstring result(message);
    LocalFree(message);
    while (!result.empty() && iswspace(result.back()))
        result.pop_back();
    return result;
}

wstring format_error(int last_error) {
    switch (last_error) {
        case 2:
        case 3:
            return L"PATH_NOT_FOUND"s;
        case 5:
            return L"ACCESS_DENIED"s;
        case 1223:
            return L"CANCELLED"s;
        default:
            return L"UNKNOWN"s;
    }
}

tuple<int, wstring, wstring> make_result(int last_error) {
    tuple<int, wstring, wstring> result(last_error, format_error(last_error), format_message(last_error));
    return result;
}