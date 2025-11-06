export type UNKNOWN = "UNKNOWN"
export type HARDDRIVE = "HARDDRIVE"
export type ROM = "ROM"
export type REMOVABLE = "REMOVABLE"
export type NETWORK = "NETWORK"
export type HOME = "HOME"

declare module 'filesystem-utilities' {

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

    export interface FileItemsResult {
        items: FileItem[]
        dirCount: number  
        fileCount: number
        path: string
    }

    export interface VersionsInput {
        path: string,
        idx: number
    }

    export interface VersionInfo {
        major: number
        minor: number
        build: number
        patch: number
    }

    export interface VersionInfoResult {
        idx: number
        info: VersionInfo
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
    export type TRASH_NOT_POSSIBLE = "TRASH_NOT_POSSIBLE"
    export type CANCELLED = "CANCELLED"
    export type FILE_EXISTS = "FILE_EXISTS"
    export type WRONG_CREDENTIALS = "WRONG_CREDENTIALS"
    export type NETWORK_NAME_NOT_FOUND = "NETWORK_NAME_NOT_FOUND"
    export type NETWORK_PATH_NOT_FOUND = "NETWORK_PATH_NOT_FOUND"
    
    
    
    export type ErrorType = ACCESS_DENIED | PATH_NOT_FOUND | TRASH_NOT_POSSIBLE| CANCELLED 
                            | FILE_EXISTS | WRONG_CREDENTIALS | NETWORK_NAME_NOT_FOUND
                            | NETWORK_PATH_NOT_FOUND | UNKNOWN

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
     * @param showHidden When 'true', retrieves hidden files too
     * @throws SystemError
     */
    function getFiles(path: string, showHidden?: boolean): Promise<FileItemsResult>
    
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
     *  * 'network-server'
     *  * 'starred'
     *  * 'android'
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
     * @param cancellation When included as string, the operation can be cancelled by calling function 'cancel' with this string as parameter
     * @returns An array of exif informations. Each entry belongs to the file path entry with the same index
     */
    function getExifInfos(files: ExifInfosInput[], cancellation?: string): Promise<ExifInfo[]>
    
    /**
     * 
     * Retrieves the file version of a exe or dll files in Windows, if included. Only available in Windows
     * @param file Pathes to the exe or dll files, together with an index.
     * @param cancellation When included as string, the operation can be cancelled by calling function 'cancel' with this string as parameter
     * @returns An array of VersionInfo informations. Each entry belongs to the file path entry with the same index
     */
    function getVersionInfos(files: VersionsInput[], cancellation?: string): Promise<VersionInfoResult[]>
    
    /**
     * Retrieves the file version info, if any, otherwise null. Only for Windows. On Linux the funtion returns null
     * @param file 
     */
    function getFileVersion(file: string): Promise<VersionInfo | null>

    /**
     * Creates a new folder 
     * @param path path of the folder to be created
     * @throws SystemError
     */    
    function createFolder(path: string): Promise<void>

    /**
     * Deletes files/a file to the trash
     * @param files the file/s to delete
     * @throws SystemError
     */
    function trash(files: string|string[]): Promise<void>

    /**
     * Copies a file
     * @param sourceFile 
     * @param targetFile 
     * @throws SystemError
     */
    function copyFile(sourceFile: string, targetFile: string): Promise<void>

    /**
     * 
     */
    type CopyOptions = {
        /**
         * error when target file exits or overwrite it?
         */
        overwrite?: boolean,
        /**
         * Moves the specified files instead of copying
         */
        move?: boolean
        /**
         * When set, you can cancel the copy process with 'cancel(cancellation)'
         */
        cancellation?: string
        /**
         * Progress callback function, only working on Linux. On Windows there is a builtin progress dialog from the shell
         * @param fileIndex index of the copied file
         * @param currentBytes current bytes copied of the current file
         * @param totalBytes total bytes of all flies to be copied
         */
        progressCallback?: (fileIndex: number, currentBytes: number, totalBytes: number) => void
    }
    
    /**
     * Copy files 'items' from 'sourcePath'>' to 'targetPath'. Sub pathes in 'items' will be copied to sub pathes in 'targetPath' (will be generated there if necessary)
     * @param sourcePath source path of items to be copied
     * @param targetPath target path of the copied items
     * @param items the file names of the items to be copied
     * @param options: options of Type CopyOptions
     * @throws SystemError
     */
    function copyFiles(sourcePath: string, targetPath: string, items: string[], options?: CopyOptions): Promise<void>
        
    /**
     * Retrieves the content of a gpx track as JSON output
     * @param path File path to a gpx track
     */
    function getGpxTrack(path: string): Promise<GpxTrack>
    
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

    /**
     * Renames a file or directory
     * @param path File path
     * @param name File name 
     * @param newName New file name
     * @throws SystemError
     */
    function rename(path: string, name: string, newName: string): Promise<void>

    /**
     * 
     * @param share connect a network share like '\\\\host\\sharename' in Windows
     * @param name the name to connect with, domain included: domain\\username
     * @param passwd the password to connect with
     * @throws SystemError with ErrorType: WRONG_CREDENTIALS, ACCESS_DENIED or NETWORK_NAME_NOT_FOUND
     */
    function addNetworkShare(share: string, name: string, passwd: string): Promise<void>
}