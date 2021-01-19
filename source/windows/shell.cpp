#include <Windows.h>
#include <algorithm>
#include "shell.h"
#include "utils.h"
#include "..\std_utils.h"
using namespace std;
using namespace Napi;

void open(const wchar_t* path) {
    SHELLEXECUTEINFOW info{0};
    info.cbSize = sizeof(SHELLEXECUTEINFOW);
    info.lpFile = path;
    info.nShow = SW_SHOW;
    info.fMask = SEE_MASK_INVOKEIDLIST;
    info.lpVerb = L"open";
    ShellExecuteExW(&info);
}

void open_as(const wchar_t* path) {
    SHELLEXECUTEINFOW info{0};
    info.cbSize = sizeof(SHELLEXECUTEINFOW);
    info.lpFile = path;
    info.nShow = SW_SHOW;
    info.fMask = SEE_MASK_INVOKEIDLIST;
    info.lpVerb = L"openas";
    ShellExecuteExW(&info);
}

void show_properties(const wchar_t* path) {
    SHELLEXECUTEINFOW info{0};
    info.cbSize = sizeof(SHELLEXECUTEINFOW);
    info.lpFile = path;
    info.nShow = SW_SHOW;
    info.fMask = SEE_MASK_INVOKEIDLIST;
    info.lpVerb = L"properties";
    ShellExecuteExW(&info);
}

void delete_directory(wstring path) {
    SHFILEOPSTRUCTW op;
    op.hwnd = nullptr;
    op.wFunc = FO_DELETE;
    path.append(L"A");
    path[path.size() - 1] = 0;
	op.pFrom = path.c_str();
    op.pTo = nullptr;
    op.fFlags = FOF_NOCONFIRMATION;
    op.fAnyOperationsAborted = FALSE;
    op.hNameMappings = nullptr;
    op.lpszProgressTitle = nullptr;
    auto result = SHFileOperationW(&op);
}

void create_directory(const wstring& path, string& error, int& error_code) {
    error_code = CreateDirectoryW(path.c_str(), nullptr) ? 0 : GetLastError();
    if (error_code == 5) {
        wchar_t temp[MAX_PATH];
        GetTempPathW(MAX_PATH, temp);
        wchar_t buffer[MAX_PATH];
        GetTempFileNameW(temp, L"xxx", 0, buffer);
        wstring tempDirectory(buffer);
		DeleteFileW(tempDirectory.c_str());
        CreateDirectoryW(tempDirectory.c_str(), nullptr);
        
        wstring newFolder(tempDirectory);
		newFolder.append(L"\\");
        auto parts = split(path, L'\\');
        newFolder.append(parts[parts.size() - 1]);
        newFolder.append(L"\\A");
        newFolder[newFolder.length() - 1] = 0;
        CreateDirectoryW(newFolder.c_str(), nullptr);
        
        SHFILEOPSTRUCTW op;
        op.hwnd = nullptr;
        op.wFunc = FO_MOVE;
		op.pFrom = newFolder.c_str();
	    wstring target = parts[0];
		for (auto i = 1; i < parts.size() - 1; i++) {
		 	target.append(L"\\");
		 	target.append(parts[i]);
		}
		target.append(L"\\A");
        target[target.length() - 1] = 0;
        op.pTo = target.c_str();
        op.fFlags = 0;
        op.fAnyOperationsAborted = FALSE;
        op.hNameMappings = nullptr;
        op.lpszProgressTitle = nullptr;
        error_code = SHFileOperationW(&op);
        delete_directory(tempDirectory);

        if (error_code != 0) {
            error = "Konnte den Ordner nicht anlegen";
            return;
        }
    }
    else if (error_code != 0)
        error = format_message(error_code).c_str();
}

void rename(wstring name, wstring new_name, string& error, int& error_code) {
    SHFILEOPSTRUCTW op;
    op.hwnd = nullptr;
    op.wFunc = FO_RENAME;
    name.append(L"\\A");
    name[name.length() - 1] = 0;
    op.pFrom = name.c_str();
    new_name.append(L"\\A");
    new_name[new_name.length() - 1] = 0;
    op.pTo = new_name.c_str();
    op.fFlags = 0;
    op.fAnyOperationsAborted = FALSE;
    op.hNameMappings = nullptr;
    op.lpszProgressTitle = nullptr;
    error_code = SHFileOperationW(&op);
    if (error_code != 0) 
        error = "Konnte nicht umbenennen";
}

void delete_files(const vector<wstring>& files, string& error, int& error_code) {
    SHFILEOPSTRUCTW op;
    op.hwnd = nullptr;
    op.wFunc = FO_DELETE;
    auto files_buffer = join(files, 0);
    files_buffer.append(L"\\A");
    files_buffer[files_buffer.length() - 1] = 0;
    op.pFrom = files_buffer.c_str();
    op.pTo = nullptr;
    op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;
    op.fAnyOperationsAborted = FALSE;
    op.hNameMappings = nullptr;
    op.lpszProgressTitle = nullptr;
    error_code = SHFileOperationW(&op);
    if (error_code != 0) 
        error = "Konnte nicht löschen";
}

void resolve_item(const wstring& source_path, const wstring& target_path, const wstring& sub_path, 
                    const WIN32_FIND_DATAW& source_info, 
                    vector<wstring>::const_iterator& exception, const vector<wstring>::const_iterator& exception_end,
                    vector<wstring>& source_items, vector<wstring>& target_items) {
    auto is_dir = (source_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
    auto path = combine_path(source_path, sub_path + L"\\" + source_info.cFileName);
    WIN32_FILE_ATTRIBUTE_DATA info;
    if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &info)) {
        if (is_dir) {
            auto dir_path = combine_path(sub_path, source_info.cFileName);
            WIN32_FIND_DATAW w32fd{ 0 };
            path = combine_path(source_path, dir_path) + L"\\*.*";
            auto ret = FindFirstFileW(path.c_str(), &w32fd);
            if (ret != INVALID_HANDLE_VALUE) {
                FindNextFileW(ret, &w32fd); // ..
                while (FindNextFileW(ret, &w32fd) == TRUE) 
                    resolve_item(source_path, target_path, dir_path, w32fd, exception, exception_end, source_items, target_items);
            }
            FindClose(ret);
        } else {
            auto file = combine_path(sub_path, source_info.cFileName);
            if (exception < exception_end && file == *exception) 
                exception++;
            else {
                source_items.push_back(combine_path(source_path, file));
                target_items.push_back(combine_path(target_path, file));
            }
        }
    }
}

auto resolve_items(wstring source_path, wstring target_path, const vector<wstring>& files, const vector<wstring>& exceptions) {
    vector<wstring> source_items;
    vector<wstring> target_items;
    auto exception_it = exceptions.cbegin();
    const auto exception_end = exceptions.cend();
    for (auto it = files.cbegin(); it < files.cend(); it++) {
        auto path = combine_path(source_path, *it);
        WIN32_FIND_DATAW w32fd{ 0 };
        auto ret = FindFirstFileW(path.c_str(), &w32fd);
        resolve_item(source_path, target_path, L"", w32fd, exception_it, exception_end, source_items, target_items);
        FindClose(ret);
    }
    return tuple(source_items, target_items);
}

void copy_files(wstring source_path, wstring target_path, const vector<wstring>& files, const vector<wstring>& exceptions,
                bool move, string& error, int& error_code) {
    SHFILEOPSTRUCTW op{0};
    op.wFunc = move ? FO_MOVE : FO_COPY;
    op.fAnyOperationsAborted = FALSE;
    if (exceptions.size() == 0) {
        vector<wstring> file_pathes;
        file_pathes.resize(files.size()); 
        transform(files.cbegin(), files.cend(), file_pathes.begin(), [source_path](wstring n){ return combine_path(source_path, n);});
        auto files_buffer = join(file_pathes, 0);
        files_buffer.append(L"A");
        files_buffer[files_buffer.length() - 1] = 0;
        op.pFrom = files_buffer.c_str();
        target_path.append(target_path[target_path.length()-1] == L'\\' ? L"A"  : L"\\A");
        target_path[target_path.length() - 1] = 0;
        op.pTo = target_path.c_str();
        op.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
        error_code = SHFileOperationW(&op);
    } else {
        auto [source_items, target_items] = resolve_items(source_path, target_path, files, exceptions);
        auto source_buffer = join(source_items, 0);
        source_buffer.append(L"A");
        source_buffer[source_buffer.length() - 1] = 0;
        op.pFrom = source_buffer.c_str();        

        auto target_buffer = join(target_items, 0);
        target_buffer.append(L"A");
        target_buffer[target_buffer.length() - 1] = 0;
        op.pTo = target_buffer.c_str();        

        op.fFlags = FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES;
        error_code = SHFileOperationW(&op);
    }
    if (error_code != 0) 
        error = move ? "Konnte nicht verschieben" : "Konnte nicht kopieren";
}
