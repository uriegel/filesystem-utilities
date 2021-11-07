#pragma once

enum class FileResult {
    Success,
    Unknown,
    AccessDenied,
    FileExists,
    FileNotFound,
    TrashNotPossible
};
