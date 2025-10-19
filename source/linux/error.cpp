#include <cerrno>
#include "../error.h"
using namespace std;

string format_message(int last_error) {
    return std::strerror(errno);
}

string format_error(int last_error) {
    switch (last_error) {
        case 2:
            return "PATH_NOT_FOUND"s;
        case 13:
            return "ACCESS_DENIED"s;
        default:
            return "UNKNOWN"s;
    }
}

string format_gerror(int g_error) {
    switch (g_error) {
        case 1:
            return "PATH_NOT_FOUND"s;
        case 14:
            return "ACCESS_DENIED"s;
        case 15:
            return "TRASH_NOT_POSSIBLE"s;
        default:
            return "UNKNOWN"s;
    }
}
