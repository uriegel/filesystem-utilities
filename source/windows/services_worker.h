#pragma once
#include <napi.h>

Napi::Value StartObservingWindowServices(const Napi::CallbackInfo& info);
Napi::Value StopObservingWindowServices(const Napi::CallbackInfo& info);