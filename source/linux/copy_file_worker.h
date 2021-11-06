#pragma once
#include <napi.h>

Napi::Value CopyFile(const Napi::CallbackInfo& info);
Napi::Value MoveFile(const Napi::CallbackInfo& info);