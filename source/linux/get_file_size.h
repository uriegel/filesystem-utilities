#pragma once
#include <napi.h>
#include <cstdint>
#include <string>

Napi::Value GetFileSizeSync(const Napi::CallbackInfo& info);
Napi::Value GetFileSize(const Napi::CallbackInfo& info);

int64_t get_file_size(const std::string& file_name);