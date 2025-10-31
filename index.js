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
            const message = inner.getErrorMessage(-e.errno)
            switch (e.errno) {
                case -13:
                    throw ({
                        error: "ACCESS_DENIED",
                        nativeError: -e.errno,
                        message
                    })
                default:
                    throw ({
                        error: "UNKNOWN",
                        nativeError: -e.errno,
                        message
                    })
            }
        }
    }

    const rename = async (filePath, name, newName) => {
        try {
            await fsa.rename(path.join(filePath, name), path.join(filePath, newName))
        } catch (e) {
            const message = inner.getErrorMessage(-e.errno)
            switch (e.errno) {
                case -13:
                    throw ({
                        error: "ACCESS_DENIED",
                        nativeError: -e.errno,
                        message
                    })
                default:
                    throw ({
                        error: "UNKNOWN",
                        nativeError: -e.errno,
                        message
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
        let copyItems = items.map(item => ({ source: path.join(sourcePath, item), target: path.join(targetPath, item) }))
        await inner.copy(copyItems, options?.progressCallback ? (idx, c, t) => options.progressCallback(idx, c, t) : (() => { }), options?.move || false, options?.overwrite || false, options?.cancellation || "")
    }

    exports.copyFile = async (source, target) => {
        await inner.copy([source, target], (c, t) => { }, false, false)
    }

    exports.createFolder = createFolder
    exports.rename = rename
    exports.getFileSizeSync = inner.getFileSizeSync
    exports.getFileSize = inner.getFileSize
    exports.getFileVersion = async () => null
    exports.getVersionInfos = async () => []
    exports.addNetworkShare = async () => { }
} else {
    exports.createFolder = inner.createFolder
    exports.getDrives = inner.getDrives
    exports.openFile = inner.openFile    
    exports.openFileWith = inner.openFileWith
    exports.showFileProperties = inner.showFileProperties
    exports.getFileVersion = inner.getFileVersion;
    exports.rename = inner.rename;
    exports.getVersionInfos = inner.getVersionInfos;
    exports.addNetworkShare = inner.addNetworkShare; 

    exports.copyFiles = async (sourcePath, targetPath, items, options) => {
        const source = items.map(n => path.join(sourcePath, n)) 
        const target = items.map(n => path.join(targetPath, n)) 
        await inner.copy(source, target, options?.move || false, options?.overwrite || false)
    }

    exports.copyFile = async (source, target) => {
        await inner.copy([source], [target], false, false)
    }
}

