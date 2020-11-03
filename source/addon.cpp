#include <napi.h>
using namespace Napi;

Object Init(Env env, Object exports) {
    return exports;    
}

const char* addon = "filesystem-utilities";
NODE_API_MODULE(addon, Init)