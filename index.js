Object.defineProperty(exports, "__esModule", { value: true });
const childProcess = require("child_process");
const path = require("path");
const fs = require('fs');
const process = require("process");
const fsa = fs.promises;
const exec = childProcess.exec;
const spawn = childProcess.spawn;

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
exports.getFileVersion = inner.getFileVersion;

if (process.platform == "linux") {
    const runCmd = cmd => new Promise<string>(res => exec(cmd, (_, stdout) => res(stdout)))

    const getDrives = async () => {
        const drivesString = await runCmd('lsblk --bytes --output SIZE,NAME,LABEL,MOUNTPOINT,FSTYPE')
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
                size: parseInt(getString(0, 1), 10)
            }
        }

        const homedir = require('os').homedir()
        const items = [{ name: "~", description: "home", mountPoint: homedir, isMounted: true, type: 1, size: 0 }]
            .concat(driveStrings
                .slice(1)
                .filter(n => n[columnsPositions[1]] > '~')
                .map(constructDrives)
        )
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
        const items = await inner.getFiles(path)
        let dirs = items.filter(n => n.isDirectory)
        let files = items.filter(n => !n.isDirectory)
        return dirs.sort((a, b) => a.name.toLowerCase().localeCompare(b.name.toLowerCase()))
            .concat(files.sort((a, b) => a.name.toLowerCase().localeCompare(b.name.toLowerCase())))
    }
    
    exports.getDrives = getDrives            
    exports.getIcon = inner.getIcon
    exports.trash = inner.trash
    exports.createFolder = createFolder
    exports.getFileSizeSync = inner.getFileSizeSync
    exports.getFileSize = inner.getFileSize
    exports.copyFile = inner.copyFile
    exports.moveFile = inner.moveFile
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
    // TODO: trash (windows)
    // TODO: copy (windows)
    // TODO: move (windows)
}
