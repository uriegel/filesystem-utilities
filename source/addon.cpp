#include <napi.h>
#if WINDOWS
#include "windows/platform.h"
#elif LINUX
#include "linux/platform.h"
#endif
#include "get_drives_worker.h"
using namespace Napi;

Object Init(Env env, Object exports) {
    exports.Set(String::New(env, "getDrives"), Function::New(env, GetDrives));
    return exports;    
}

const char* addon = "filesystem-utilities";
NODE_API_MODULE(addon, Init)