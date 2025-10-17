#pragma once
#include <napi.h>

Napi::Value OpenFile(const Napi::CallbackInfo &info);
Napi::Value OpenFileWith(const Napi::CallbackInfo &info);
Napi::Value ShowFileProperties(const Napi::CallbackInfo &info);