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

declare enum TrashErr {
    FileNotFound = 1,
    PermissionDenied = 14
}

declare module 'filesystem-utilities' {
    function getDrives(): Promise<DriveItem[]>
    function getFiles(directory: string): Promise<FileItem[]>
    function getIcon(ext: string): Promise<string|Buffer>
    function getExifDate(file: string): Promise<Date|null>
    function getFileVersion(file: string): Promise<VersionInfo|null>
    function trash(pathes: string| string[]): Promise<void>
}