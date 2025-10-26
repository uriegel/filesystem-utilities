#include <napi.h>
#pragma once
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include <gtk/gtk.h>
    #include "linux/platform.h"
#endif
std::tuple<int, stdstring, stdstring> make_result(int last_error);
#if LINUX
std::tuple<int, stdstring, stdstring> make_result(int last_error, GError* gerror);
std::tuple<int, stdstring, stdstring> make_cancelled_result();
Napi::Value GetErrorMessage(const Napi::CallbackInfo &info);
#else
std::tuple<int, stdstring, stdstring> make_move_not_possible();
#endif