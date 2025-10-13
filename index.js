Object.defineProperty(exports, "__esModule", { value: true });
const childProcess = require("child_process");
const fs = require('fs');
const process = require("process");
const fsa = fs.promises;
const exec = childProcess.exec;

// TODO: Test in Commander

// TODO: cancelling getExifInfosAsync with requestId
// TODO: readTrack: add missing fields
// TODO: DateTime always as javascript Date, it converts correctly to json

// TODO: Windows version: getFiles with exceptions
// TODO: Windows version: getTrackInfo string <-> wstring <-> char*

var FileResult;
(function (FileResult) {
    FileResult[FileResult["Success"] = 0] = "Success";
    FileResult[FileResult["Unknown"] = 1] = "Unknown";
    FileResult[FileResult["AccessDenied"] = 2] = "AccessDenied";
    FileResult[FileResult["FileExists"] = 3] = "FileExists";
    FileResult[FileResult["FileNotFound"] = 4] = "FileNotFound";
    FileResult[FileResult["TrashNotPossible"] = 5] = "TrashNotPossible";
})(FileResult = exports.FileResult || (exports.FileResult = {}));

const requireAddon = () => {
    try {
        return require("./build/Release/filesystem-utilities")        
    } catch (err) {
        return require('bindings')('filesystem-utilities')
    }
}

const inner = requireAddon()
exports.getExifDate = inner.getExifDate;
exports.getExifInfosAsync = inner.getExifInfosAsync;
exports.getFileVersion = inner.getFileVersion;
exports.trash = inner.trash
exports.copy = inner.copy

if (process.platform == "linux") {
    const runCmd = cmd => new Promise(res => exec(cmd, (_, stdout) => res(stdout)))

    const getDrives = async () => {
        const drivesString = (await runCmd('lsblk --bytes --output SIZE,NAME,LABEL,MOUNTPOINT,FSTYPE'))
        const driveStrings = drivesString.split("\n")
        const columnsPositions = (() => {
            const title = driveStrings[0]
            const getPart = key => title.indexOf(key)

            return [
                0,
                getPart("NAME"),
                getPart("LABEL"),
                getPart("MOUNT"),
                getPart("FSTYPE")
            ]
        })()

        //const takeOr = (text: string, alt: string) => text ? text : alt
        const constructDrives = driveString => {
            const getString = (pos1, pos2) =>
                driveString.substring(columnsPositions[pos1], columnsPositions[pos2]).trim()
            const trimName = name =>
                name.length > 2 && name[1] == '─'
                    ? name.substring(2)
                    : name
            const mount = getString(3, 4)
         
            return {
                description: getString(2, 3),
                name: trimName(getString(1, 2)),
                type: 1, // TODO: Drive types enum DriveType
                mountPoint: mount,
                isMounted: !!mount,
                driveType: driveString.substring(columnsPositions[4]).trim(),
                size: parseInt(getString(0, 1), 10),
                isRoot: driveString[columnsPositions[1]] < '~'
            }
        }

        const homedir = require('os').homedir()
        const itemOffers = [{ name: "~", description: "home", mountPoint: homedir, isMounted: true, type: 1, isRoot: false }]
            .concat(driveStrings
                .slice(1)
                .map(constructDrives)
        )
        const items = itemOffers
                        .filter(rio => (!rio.isRoot && rio.name) || (itemOffers.filter(n => n.name != rio.name && n.name.startsWith(rio.name)) == 0
                            && rio.mountPoint != "[SWAP]"))
            .map(n => ({
                description: n.description, name: n.name, type: n.type, mountPoint: n.mountPoint, isMounted: n.isMounted, driveType: n.driveType, size: n.size
            }))
        
        const mounted = items.filter(n => n.isMounted)
        const unmounted = items.filter(n => !n.isMounted)
        return mounted.concat(unmounted)
    }    

    const createFolder = async path => {
        try {
            await fsa.mkdir(path, { recursive: true })   
        } catch (e) {
            switch (e.errno) {
                case -13:
                    throw ({
                        fileResult: FileResult.AccessDenied,
                        description: e.stack
                    })
                case -17:
                    throw ({
                        fileResult: FileResult.FileExists,
                        description: e.stack
                    })
                default:
                    throw ({
                        fileResult: FileResult.Unknown,
                        description: "Unknown error occurred"
                    })
            }
        }
    }

    exports.getFiles = async path => {
        const items = await inner.getFiles(path, true)
        let dirs = items.filter(n => n.isDirectory)
        let files = items.filter(n => !n.isDirectory)
        return dirs.sort((a, b) => a.name.toLowerCase().localeCompare(b.name.toLowerCase()))
            .concat(files.sort((a, b) => a.name.toLowerCase().localeCompare(b.name.toLowerCase())))
    }
    
    exports.getFilesAsync = async (path, isHidden) => {
        const fileItems = await inner.getFiles(path, isHidden == true)
        let dirs = fileItems.filter(n => n.isDirectory)
        let files = fileItems.filter(n => !n.isDirectory)
        let dirCount = dirs.length
        let fileCount = files.length
        const items =  dirs.sort((a, b) => a.name.toLowerCase().localeCompare(b.name.toLowerCase()))
            .concat(files.sort((a, b) => a.name.toLowerCase().localeCompare(b.name.toLowerCase())))
        return {
            dirCount,
            fileCount,
            path,
            items
        }
    }

    exports.getDrives = getDrives            
    exports.getIcon = inner.getIcon
    exports.createFolder = createFolder
    exports.getFileSizeSync = inner.getFileSizeSync
    exports.getFileSize = inner.getFileSize
    exports.getGpxTrackAsync = inner.getGpxTrackAsync
} else {
    const createFolder = async path => {
        try {
            await inner.createDirectory(path)
        } catch (e) {
            switch (e.code) {
                case -13:
                    throw ({
                        res: FileResult.AccessDenied,
                        description: e.description
                    })
                case 183:
                    throw ({
                        res: FileResult.FileExists,
                        description: e.description
                    })
                default:
                    throw ({
                        res: FileResult.Unknown,
                        description: e.description
                    })
            }
        }
    }
    exports.getFiles = inner.getFiles
    exports.getDrives = inner.getDrives
    exports.getIcon = inner.getIcon
    exports.createFolder = createFolder
    // TODO: echo "password" | sudo -S -s -- nautilus
}
