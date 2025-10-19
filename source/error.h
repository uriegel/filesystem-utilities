#pragma once
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include "linux/platform.h"
#endif
stdstring format_message(int last_error); 
stdstring format_error(int last_error);
stdstring format_gerror(int g_error);
