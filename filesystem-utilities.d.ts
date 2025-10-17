export type UNKNOWN = "UNKNOWN"
export type HARDDRIVE = "HARDDRIVE"
export type ROM = "ROM"
export type REMOVABLE = "REMOVABLE"
export type NETWORK = "NETWORK"
export type HOME = "HOME"

export type DriveType = UNKNOWN | HARDDRIVE | ROM | REMOVABLE | NETWORK | HOME

export interface DriveItem {
    name: string
    description: string
    size?: number
    type: DriveType 
    mountPoint?: string
    isMounted?: boolean
}

export interface FileItem {
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
        heartrate: number
        velocity: number
    }

    export interface GpxTrack {
        name: string
        distance: number
        duration: number
        date: string
        averageSpeed: number
        trackPoints: GpxPoint[]
    }

    export type ACCESS_DENIED = "ACCESS_DENIED"
    export type PATH_NOT_FOUND = "PATH_NOT_FOUND"
    
    export type ErrorType = ACCESS_DENIED | PATH_NOT_FOUND | UNKNOWN

    export interface SystemError {
        error: ErrorType,
        nativeError: number,
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
    /**
     * Gets drives Retrieves all direves from the file system
     * @returns drives 
     */
    function getDrives(): Promise<DriveItem[]>

    /**
     * Retrieves all files from a directory. 
     * @param path parent directory containing directories and files to be retrieved
     * @deprecated use getFilesAsync instead
     */
    function getFiles(directory: string): Promise<FileItem[]>

    /**
     * Retrieves all files from a directory. 
     * @param path parent directory containing directories and files to be retrieved
     * @param showHidden When 'true', retrieves hidden files too
     * @throws ErrorType
     */
    function getFilesAsync(path: string, showHidden?: boolean): Promise<FileItemsResult>
    
    /**
     * Retrieves system icon in 16x16, as png or svg for dedicated file extensions
     * @param ext The file extension to retrieve the icon for. like '.mp4'
     * @result The icon as binary data
     */
    function getIcon(ext: string): Promise<Buffer>
    
    /**
     * Retrieves icon in 16x16, as png or svg for a special name. 
     * On linux these are the names GTK can look up an deliver.
     * On Windows only specific names are supported:
     *  * 'drive-removable-media'
     *  * 'media-removable'
     *  * 'drive-windows'
     *  * 'folder-open'
     *  * 'user-home'
     *  * 'go-up'
     * @result The icon as binary data
     */
    function getIconFromName(name: string): Promise<Buffer>
    
    /**
     * Retrieves the exif date of a png or jpg file, if included
     * @param file Path to the png or jpg file
    */
    function getExifDate(file: string): Promise<Date | null>
    
    /**
     * 
     * Retrieves the exif datas of a png or jpg file, if included
     * @param file Pathes to the png or jpg files, together with an index.
     * @param cancellation When included as string, the oeration can be cancelled by calling function 'cancel' with this string as parameter
     * @returns An array of exif informations. Each entry belongs to the file path entry with the same index
     */
    function getExifInfosAsync(files: ExifInfosInput[], cancellation?: string): Promise<ExifInfo[]>
    
    
    function getFileVersion(file: string): Promise<VersionInfo | null>
    function trash(path: string | string[]): Promise<void>
    function createFolder(path: string): Promise<void>
    function copyFile(source: string, target: string, progress: (currentSize: number, totalSize: number)=>void, overwrite?: boolean): Promise<void>
    function moveFile(source: string, target: string, progress: (currentSize: number, totalSize: number) => void, overwrite?: boolean): Promise<void>    
    
    /**
     * Retrieves the content of a gpx track as JSON output
     * @param path File path to a gpx track
     */
    function getGpxTrackAsync(path: string): Promise<GpxTrack>
    
     /**
     * Cancels an operation by calling this function with the cancellation token you have also set in the operation to cancel
     */
    function cancel(cancellation: string): void

    /**
     * Opens a file
     * @param path File path
     */
    function openFile(path: string): void    

    /**
     * Opens a file in Windows by showing "open with" dialog
     * @param path File path
     */
    function openFileWith(path: string): void    

    /**
     * Shows file properties in Windows
     * @param path File path
     */
    function showFileProperties(path: string): void    
}