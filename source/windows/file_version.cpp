#include <Windows.h>
#include "../file_version.h"
using namespace std;

optional<Version_info> get_file_info_version(const wstring& file_name) {
    DWORD _{0};
    auto size = GetFileVersionInfoSizeW(file_name.c_str(), &_);
    if (size == 0)
        return nullopt;
	vector<char> data(size);
	auto ok = GetFileVersionInfoW(file_name.c_str(), 0, size, data.data());
	VS_FIXEDFILEINFO *info{nullptr};
	uint32_t len{0};
	VerQueryValueW(data.data(), L"\\", reinterpret_cast<void**>(&info), &len);

    return make_optional<Version_info>(
        HIWORD(info->dwFileVersionMS),
        LOWORD(info->dwFileVersionMS),
        HIWORD(info->dwFileVersionLS),
        LOWORD(info->dwFileVersionLS)
	);
}