#include <windows.h>
#include <array>
#include <vector>
#include "drives.h"
#include "../std_utils.h"
using namespace std;

class file_handle
{
public:
	file_handle(HANDLE handle) : handle(handle) {}
	~file_handle() {
		if (handle != INVALID_HANDLE_VALUE)
			CloseHandle(handle);
	}
	operator HANDLE() const { return handle; }
private:
	HANDLE handle{ INVALID_HANDLE_VALUE };
};

const wstring get_drive_description(const wstring& name) {
	array<wchar_t, 400> buffer;
	if (GetVolumeInformationW(name.c_str(), buffer.data(), static_cast<DWORD>(buffer.size()), nullptr, nullptr, nullptr, nullptr, 0))
		return wstring(buffer.data(), wcslen(buffer.data()));
	else
		return wstring();
}

uint64_t get_volume_size(const wstring& directory_on_drive) {
	ULARGE_INTEGER result{ 0 };
	GetDiskFreeSpaceExW(directory_on_drive.c_str(), nullptr, &result, nullptr);
	return result.QuadPart;
}

Drive_type get_drive_type(unsigned int type) {
	switch (type)
	{
	case 2:
		return Drive_type::REMOVABLE;
	case 3:
		return Drive_type::HARDDRIVE;
	case 4:
		return Drive_type::NETWORK;
	case 5:
		return Drive_type::ROM;
	default:
		return Drive_type::UNKNOWN;
	}
}

bool is_mounted(const file_handle& volume) {
	if (volume == INVALID_HANDLE_VALUE)
		// 2 means "no disk", anything else means by inference "disk
		// in drive, but you do not have admin privs to do a
		// CreateFile on that volume".
		return GetLastError() != 2;

	DWORD bytes_returned; // ignored
	auto result = DeviceIoControl(volume, FSCTL_IS_VOLUME_MOUNTED, nullptr, 0, nullptr, 0, &bytes_returned, nullptr);
	return result != 0;
}

vector<Drive_item> get_drives() {
  	array<wchar_t, 500> buffer;
	auto size = GetLogicalDriveStringsW(static_cast<DWORD>(buffer.size()), buffer.data());
	const wstring drive_string(buffer.data(), size);
	auto drives = split(drive_string, 0);

	vector<Drive_item> result;
	transform(drives.begin(), drives.end(), back_inserter(result), [](const wstring & val) {
		auto type = GetDriveTypeW(val.c_str());
		auto volume = wstring{ L"\\\\.\\" + val.substr(0, 2) };
		file_handle volume_handle(CreateFileW(volume.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr, OPEN_EXISTING, 0, nullptr));
		return move(Drive_item
			{
				val,
				move(get_drive_description(val)),
				get_volume_size(val),
				get_drive_type(type),
				type != 3 ? is_mounted(volume_handle) : true
			});
		});

	// auto erase_it = remove_if(drive_items.begin(), drive_items.end(), [](const Drive_item & val) {
	// 	return !val.is_mounted;
	// 	});
	// drive_items.erase(erase_it, drive_items.end());
	return result;
}

string drivetype_to_string(Drive_type dt) {
	switch (dt) {
		case Drive_type::UNKNOWN:
			return "UNKNOWN"s;
		case Drive_type::HARDDRIVE:
			return "HARDDRIVE"s;
		case Drive_type::ROM:
			return "ROM"s;
		case Drive_type::REMOVABLE:
			return "REMOVABLE"s;
		case Drive_type::NETWORK:
			return "NETWORK"s;
		default:
			return "UNKNOWN"s;
	}
}
