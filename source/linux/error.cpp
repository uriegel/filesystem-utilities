#include <cerrno>
#include <iostream>
#include "../error.h"
using namespace Napi;
using namespace std;

string format_message(int last_error) {
    return std::strerror(last_error);
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
        case 2:
            return "FILE_EXISTS"s;
        case 14:
            return "ACCESS_DENIED"s;
        case 15:
            return "TRASH_NOT_POSSIBLE"s;
        default:
            return "UNKNOWN"s;
    }
}

string format_g_message(int last_error, int g_error, string message) {
    cerr << "Error occurred: " << message.c_str() << endl;
    switch (g_error) {
        case 1:
            return "Datei nicht gefunden"s;
        case 2:
            return "Zieldatei existiert bereits"s;
        case 14:
            return "Zugriff verweigert"s;
        case 15:
            return "Löschen in den Papierkorb nicht möglich"s;
        case 25:
            return "Rekursives Kopieren nicht möglich"s;
        default:
            return format_message(last_error);
    }
}

tuple<int, string, string> make_cancelled_result() {
    tuple<int, string, string> result(-1, "CANCELLED", "Die Aktion wurde abgebrochen");
    return result;
}

tuple<int, string, string> make_result(int last_error) {
    tuple<int, string, string> result(errno, format_error(last_error), format_message(last_error));
    return result;
}

tuple<int, string, string> make_result(int last_error, GError* gerror) {
    tuple<int, string, string> result(errno, format_gerror(gerror->code), format_g_message(last_error, gerror->code, gerror->message));
    return result;
}

Value GetErrorMessage(const CallbackInfo &info) {
    auto err = info[0].As<Number>().Int32Value();
    auto msg = format_message(err);
    return nodestring::New(info.Env(), msg);
}