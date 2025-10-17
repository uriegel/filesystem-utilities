#include "process_file.h"
#include "platform.h"
#include "shell.h"
using namespace std;
using namespace Napi;

Value OpenFile(const CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto path = (wstring)info[0].As<nodestring>();
    open(path.c_str());
    return env.Undefined();    
}

Value OpenFileWith(const CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto path = (wstring)info[0].As<nodestring>();
    open_as(path.c_str());
    return env.Undefined();    
}

Value ShowFileProperties(const CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto path = (wstring)info[0].As<nodestring>();
    show_properties(path.c_str());
    return env.Undefined();    
}
