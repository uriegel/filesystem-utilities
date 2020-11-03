#include "nullfunction.h"
using namespace Napi;

String NullFunction(const Napi::CallbackInfo& info) { return String::New(info.Env(), ""); }