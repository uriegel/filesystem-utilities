#include <napi.h>
#if WINDOWS
#include "windows/platform.h"
#elif LINUX
#include "linux/platform.h"
#endif
#include "get_drives_worker.h"
#include "get_files_worker.h"
#include "get_icon_worker.h"
using namespace Napi;

Object Init(Env env, Object exports) {
    exports.Set(String::New(env, "getDrives"), Function::New(env, GetDrives));
    exports.Set(String::New(env, "getFiles"), Function::New(env, GetFiles));
    exports.Set(String::New(env, "getIcon"), Function::New(env, GetIcon));
    return exports;    
}

const char* addon = "_filesystem-utilities";
NODE_API_MODULE(addon, Init)