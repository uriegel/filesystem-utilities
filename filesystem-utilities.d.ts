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
    size?: number
    type: DriveType 
    mountPoint?: string
}

interface FileItem {
    name: string
    idx: number
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

    export interface FileItemsResult {
        items: FileItem[]
        dirCount: number  
        fileCount: number
        path: string
    }

    export interface GpxPoint {
        lat: number
        lon: number
        ele: number
        time: string
    }

    export interface GpxTrack {
        name: string
        distance: number
        duration: number
        date: string
        trackPoints: GpxPoint[]
    }

    export interface ErrorType {
        code: number,
        message: string
    }

    export interface ExifInfosInput {
        path: string,
        idx: number
    }

    export interface ExifInfo {
        idx: number,
        date: Date,
        latitude: number,
        longitude: number
    }

    function getDrives(): Promise<DriveItem[]>
    function getFiles(directory: string): Promise<FileItem[]>
    function getFilesAsync(path: string, isHidden?: boolean): Promise<FileItemsResult>
    function getIcon(ext: string): Promise<Buffer>
    function getExifDate(file: string): Promise<Date | null>
    function getExifInfosAsync(files: ExifInfosInput[]): Promise<ExifInfo[]>
    function getFileVersion(file: string): Promise<VersionInfo|null>
    function trash(path: string | string[]): Promise<void>
    function createFolder(path: string): Promise<void>
    function copyFile(source: string, target: string, progress: (currentSize: number, totalSize: number)=>void, overwrite?: boolean): Promise<void>
    function moveFile(source: string, target: string, progress: (currentSize: number, totalSize: number) => void, overwrite?: boolean): Promise<void>    
    function getGpxTrackAsync(path: string): Promise<GpxTrack>
}