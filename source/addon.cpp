#include <napi.h>
#if WINDOWS
    #include "windows/platform.h"
    #include "windows/get_drives_worker.h"
    #include "windows/get_file_version_worker.h"
    #include "windows/create_directory_worker.h"
    #include "windows/copy_worker.h"
#elif LINUX
    #include "linux/platform.h"
    #include "linux/copy_worker.h"
#endif
#include "get_files_worker.h"
#include "get_icon_worker.h"
#include "get_exif_date_worker.h"
#include "get_exif_infos_worker.h"
#include "trash_worker.h"
#include "get_gpx_track_worker.h"
using namespace Napi;

Object Init(Env env, Object exports) {
#if LINUX
    std::setlocale(LC_MESSAGES, "");
    std::setlocale(LC_CTYPE, "");
#endif
    exports.Set(String::New(env, "getFiles"), Function::New(env, GetFiles));
    exports.Set(String::New(env, "getIcon"), Function::New(env, GetIcon));
    exports.Set(String::New(env, "getExifDate"), Function::New(env, GetExifDate));
    exports.Set(String::New(env, "getExifInfosAsync"), Function::New(env, GetExifInfosAsync));
    exports.Set(String::New(env, "trash"), Function::New(env, Trash));
    exports.Set(String::New(env, "copy"), Function::New(env, Copy));
    exports.Set(String::New(env, "getGpxTrackAsync"), Function::New(env, GetGpxTrack));
#if LINUX
#endif
#if WINDOWS    
    exports.Set(String::New(env, "getDrives"), Function::New(env, GetDrives));
    exports.Set(String::New(env, "getFileVersion"), Function::New(env, GetFileVersion));
    exports.Set(String::New(env, "createDirectory"), Function::New(env, CreateDirectory1));
#endif
    return exports;    
}

const char* addon = "_filesystem-utilities";
NODE_API_MODULE(addon, Init)