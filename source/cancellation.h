#include <napi.h>
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include "linux/platform.h"
#endif

void register_cancellable(stdstring cancellation);
void unregister_cancellable(stdstring cancellation);
bool is_cancelled(stdstring cancellation);
void cancel(stdstring cancellation);
Napi::Value Cancel(const Napi::CallbackInfo& info);