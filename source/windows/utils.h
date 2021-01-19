#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <optional>

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

std::wstring combine_path(std::wstring path, const std::wstring& path_to_combine);
std::string format_message(int last_error); 
void start_elevated();
