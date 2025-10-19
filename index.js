Object.defineProperty(exports, "__esModule", { value: true })
const childProcess = require("child_process")
const fs = require('fs')
const path = require('path')
const process = require("process")
const fsa = fs.promises
const exec = childProcess.exec
const spawn = childProcess.spawn 

const requireAddon = () => {
    try {
        return require("./build/Release/filesystem-utilities")        
    } catch (err) {
        return require('bindings')('filesystem-utilities')
    }
}

const inner = requireAddon()
exports.getExifDate = inner.getExifDate;
exports.getExifInfos = inner.getExifInfosAsync;
exports.getFileVersion = inner.getFileVersion;
exports.cancel = inner.cancel
exports.getIconFromName = inner.getIconFromName
exports.getIcon = inner.getIcon
exports.getGpxTrack = inner.getGpxTrackAsync

exports.getFiles = async (path, showHidden) => {
    const fileItems = await inner.getFiles(path, showHidden == true)
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

exports.trash = async files => {
    await inner.trash(Array.isArray(files) ? files : [files])
}

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
                type: "HARDDRIVE", // TODO: Drive types enum DriveType
                mountPoint: mount,
                isMounted: !!mount,
                driveType: driveString.substring(columnsPositions[4]).trim(),
                size: parseInt(getString(0, 1), 10),
                isRoot: driveString[columnsPositions[1]] < '~'
            }
        }

        const homedir = require('os').homedir()
        const itemOffers = [{ name: "~", description: "home", mountPoint: homedir, isMounted: true, type: "HOME", isRoot: false }]
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

    exports.openFile = path => {
    	spawn("xdg-open", [`${path}`])   
    }

    exports.openFileWith = () => { }
    exports.showFileProperties = () => { }
    
    exports.getDrives = getDrives    
    
    exports.copyFiles = async (sourcePath, targetPath, items, options) => {
        let idx = 0        
        for (const item of items) {
            const source = path.join(sourcePath, item)
            const target = path.join(targetPath, item)
            await inner.copy(source, target, options?.progressCallback ? (c, t) => options.progressCallback(idx, c, t) : ((c, t) => { }))
            idx++
        }
    }

    exports.createFolder = createFolder
    exports.getFileSizeSync = inner.getFileSizeSync
    exports.getFileSize = inner.getFileSize
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
    exports.getDrives = inner.getDrives

    exports.copyFiles = async (sourcePath, targetPath, items, options) => {
        const source = items.map(n => path.join(sourcePath, n)) 
        const target = items.map(n => path.join(targetPath, n)) 
        await inner.copy(source, target)
    }

    exports.createFolder = createFolder
    exports.openFile = inner.openFile    
    exports.openFileWith = inner.openFileWith
    exports.showFileProperties = inner.showFileProperties
}
// TODO copy already exists
// TODO copy access denied
// TODO copy path not found
// TODO copy move
// TODO copy overwrite

// TODO getFileVersion (s)
// TODO createFolder
// TODO rename
