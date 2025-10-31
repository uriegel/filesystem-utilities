#pragma once
#include <napi.h>

struct VersionsInput {
    int idx;
    std::wstring path;
};

struct VersionInfo {
    int major;
    int minor;
    int build;
    int patch;
};

struct VersionInfoResult {
    int idx;
    VersionInfo info;
};

Napi::Value GetVersions(const Napi::CallbackInfo& info);