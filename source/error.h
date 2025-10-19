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
#endif