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
            return "GENERAL"s;
    }
}
