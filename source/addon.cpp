#include <napi.h>
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include "linux/platform.h"
#endif
#include "initialize.h"
#include "get_drives_worker.h"
#include "get_files_worker.h"
#include "get_icon_worker.h"
#include "get_exif_date_worker.h"
#include "get_file_version_worker.h"
using namespace Napi;

Object Init(Env env, Object exports) {
    initialize();
    exports.Set(String::New(env, "getDrives"), Function::New(env, GetDrives));
    exports.Set(String::New(env, "getFiles"), Function::New(env, GetFiles));
    exports.Set(String::New(env, "getIcon"), Function::New(env, GetIcon));
    exports.Set(String::New(env, "getExifDate"), Function::New(env, GetExifDate));
    exports.Set(String::New(env, "getFileVersion"), Function::New(env, GetFileVersion));
    return exports;    
}

const char* addon = "_filesystem-utilities";
NODE_API_MODULE(addon, Init)