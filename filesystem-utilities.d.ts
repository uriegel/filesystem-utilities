declare enum DriveType {
	UNKNOWN,
	HARDDRIVE,
	ROM,
	REMOVABLE,
	NETWORK
}

interface DriveItem {
    name: string
    description: string
    size: number
    type: DriveType 
    mountPoint?: string
}

interface FileItem {
    name: string
    isDirectory: boolean
    isHidden?: boolean
    size?: number 
    time?: Date
}

interface VersionInfo {
    major: number
    minor: number
    build: number
    patch: number
}

declare module 'filesystem-utilities' {
    enum FileResult {
        Success,
        Unknown,
        AccessDenied,
        FileExists,
        FileNotFound,
        TrashNotPossible
    }
    interface FileException {
        res: number
        description: FileResult
    }
    function getDrives(): Promise<DriveItem[]>
    function getFiles(directory: string): Promise<FileItem[]>
    function getIcon(ext: string): Promise<Buffer>
    function getExifDate(file: string): Promise<Date|null>
    function getFileVersion(file: string): Promise<VersionInfo|null>
    function trash(path: string | string[]): Promise<void>
    function createFolder(path: string): Promise<void>
    function copyFile(source: string, target: string, progress: (currentSize: number, totalSize: number)=>void, overwrite?: boolean): Promise<void>
    function moveFile(source: string, target: string, progress: (currentSize: number, totalSize: number)=>void, overwrite?: boolean): Promise<void>    
}