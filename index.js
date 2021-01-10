const process = require("process")
const path = require("path")
const fs = require('fs')
const childProcess = require("child_process")
const fsa = fs.promises

const exec = childProcess.exec
const spawn = childProcess.spawn

var FileResult
(function (FileResult) {
    FileResult[FileResult["Success"] = 0] = "Success"
    FileResult[FileResult["Unknown"] = 1] = "Unknown"
    FileResult[FileResult["AccessDenied"] = 2] = "AccessDenied"
    FileResult[FileResult["FileExists"] = 3] = "FileExists"
    FileResult[FileResult["FileNotFound"] = 4] = "FileNotFound"
})(FileResult || (FileResult = {}))

const requireAddon = () => {
    try {
        return require("./build/Release/filesystem-utilities")        
    } catch (err) {
        return require('bindings')('filesystem-utilities')
    }
}

const inner = requireAddon()
exports.FileResult = FileResult
exports.getFiles = inner.getFiles
exports.getExifDate = inner.getExifDate
exports.getFileVersion = inner.getFileVersion

if (process.platform == "linux") {

    const runCmd = cmd => new Promise(res => exec(cmd, (_, stdout) => res(stdout)))

    const getDrives = async () => {
        const drivesString = await runCmd('lsblk --bytes --output SIZE,NAME,LABEL,MOUNTPOINT,FSTYPE')
        const driveStrings = drivesString.split("\n")
        const columnsPositions = (() => {
            const title = driveStrings[0]
            const getPart = (key) => title.indexOf(key) 

            return [ 
                0,
                getPart("NAME"), 
                getPart("LABEL"),
                getPart("MOUNT"),
                getPart("FSTYPE")
            ]
        })()         

        const takeOr = (text, alt) => text ? text : alt
        const constructDrives = (driveString) => {
            const getString = (pos1, pos2) => 
                driveString.substring(columnsPositions[pos1], columnsPositions[pos2]).trim()
            const trimName = name => 
                name.length > 2 && name[1] == '─' 
                ? name.substring(2)
                : name
            const mount = getString(3, 4)
            
            return {
                name: takeOr(getString(2, 3), mount),
                description: trimName(getString(1, 2)),
                type: 1, // TODO: Drive types enum DriveType
                mountPoint: mount,
                driveType: driveString.substring(columnsPositions[4]).trim(),
                size: parseInt(getString(0, 1), 10)
            }
        }   

        return driveStrings
            .slice(1)
            .map(constructDrives)
            .filter(n => n.mountPoint && !n.mountPoint.startsWith("/snap"))        
    }

    const getIcon = ext => new Promise((res, rej) => {
        const process = spawn('python3',[ path.join(__dirname, "getIcon.py"), ext ])
        process.stdout.on('data', data => {
            const icon = data.toString('utf8').trim()
            res(icon)
        })
        process.stderr.on('data', data =>  rej(data.toString('utf8').trim()))
    })

    const trash = async pathes => {
        if (Array.isArray(pathes))
            pathes.forEach(async n => await trashOneFile(n))
        else
            await trashOneFile(pathes)
    }

    const trashOneFile = file => new Promise((res, rej) => {
        const process = spawn('python3',[ path.join(__dirname, "delete.py"), file ])
        process.stdout.on('data', data => {
            const icon = data.toString('utf8').trim()
            res()
        })
        process.stderr.on('data', data =>  {
            const z = data.toString('utf8').trim()
            const err = Number.parseInt(z)
            switch (err) {
                case 1:
                    rej({
                        res: FileResult.FileNotFound,
                        description: "file not found"
                    })    
                    break
                case 15:
                    rej({
                        res: FileResult.AccessDenied,
                        description: "Access denied"
                    })    
                    break
                default:
                rej({
                    res: FileResult.Unknown,
                    description: "Unknown error occurred"
                })
                break
            }
        })
    })

    const createFolder = async path => {
        try {
            await fsa.mkdir(path)   
        } catch (e) {
            switch (e.errno) {
                case -13:
                    throw ({
                        res: FileResult.AccessDenied,
                        description: e.stack
                    })
                    break
                case -17:
                    throw ({
                        res: FileResult.FileExists,
                        description: e.stack
                    })
                    break
                default:
                    throw ({
                        res: FileResult.Unknown,
                        description: "Unknown error occurred"
                    })
                    break
            }
        }
    }

    const copy = (source, target, progress) => {
        return new Promise(res => {
            const process = spawn('cp' ,[source, target])    
            const progressId = setInterval(async () => {
                const progressResult = await runCmd(`progress -p ${process.pid}`)
                const percentage = 
                    progressResult.split('\n')
                    .filter(n => n.includes('% ('))
                    .map(n => {
                        const words = n.split('%').map(n => n.trim())
                        return words[0]
                    })[0]
                    progress(percentage)
                if (percentage == "100.0")
                    clearInterval(progressId)
            }, 1000)
            process.once("exit", e => {
                clearInterval(progressId)
                res()
            })
        })
    }
    

    exports.getDrives = getDrives            
    exports.getIcon = getIcon
    exports.trash = trash
    exports.createFolder = createFolder
    exports.copy = copy
}
else {
    exports.getDrives = inner.getDrives
    exports.getIcon = inner.getIcon
    // TODO: trash for windows
    // TODO: createFolder for windows
    // TODO: copy for windows
}



