#include <windows.h>
#include <array>
#include <vector>
#include "../drives.h"
#include "../std_utils.h"
using namespace std;

// TODO: shared!
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
