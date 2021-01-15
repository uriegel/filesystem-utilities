const childProcess = require("child_process")
const path = require("path")
const fs = require('fs')
const process = require("process")

const fsa = fs.promises
const exec = childProcess.exec
const spawn = childProcess.spawn

export enum FileResult {
    Success,
    Unknown,
    AccessDenied,
    FileExists,
    FileNotFound
}

const requireAddon = () => {
    try {
        return require("./build/Release/filesystem-utilities")        
    } catch (err) {
        return require('bindings')('filesystem-utilities')
    }
}

const inner = requireAddon()
export const getFiles = inner.getFiles
export const getExifDate = inner.getExifDate
export const getFileVersion = inner.getFileVersion

if (process.platform == "linux") {
    const runCmd = (cmd: string) => new Promise<string>(res => exec(cmd, (_: any, stdout: any) => res(stdout)))

    const getDrives = async () => {
        const drivesString = await runCmd('lsblk --bytes --output SIZE,NAME,LABEL,MOUNTPOINT,FSTYPE')
        const driveStrings = drivesString.split("\n")
        const columnsPositions = (() => {
            const title = driveStrings[0]
            const getPart = (key: string) => title.indexOf(key) 

            return [ 
                0,
                getPart("NAME"), 
                getPart("LABEL"),
                getPart("MOUNT"),
                getPart("FSTYPE")
            ]
        })()         

        const takeOr = (text: string, alt: string) => text ? text : alt
        const constructDrives = (driveString: string) => {
            const getString = (pos1: number, pos2: number) => 
                driveString.substring(columnsPositions[pos1], columnsPositions[pos2]).trim()
            const trimName = (name: string) => 
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

    const getIcon = (ext: string) => new Promise((res, rej) => {
        const process = spawn('python3',[ path.join(__dirname, "getIcon.py"), ext ])
        process.stdout.on('data', (data: any) => {
            const icon = data.toString('utf8').trim()
            res(icon)
        })
        process.stderr.on('data', (data: any) =>  rej(data.toString('utf8').trim()))
    })

    const trash = async (pathes: string[]) => {
        if (Array.isArray(pathes)) 
            for (const path of pathes) 
                await trashOneFile(path)
        else
            await trashOneFile(pathes)
    }

    const trashOneFile = (file: string) => new Promise<void>((res, rej) => {
        const process = spawn('python3',[ path.join(__dirname, "delete.py"), file ])
        process.stdout.on('data', () => res())
        process.stderr.on('data', (data: any) =>  {
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

    const createFolder = async (path: string) => {
        try {
            await fsa.mkdir(path)   
        } catch (e) {
            switch (e.errno) {
                case -13:
                    throw ({
                        res: FileResult.AccessDenied,
                        description: e.stack
                    })
                case -17:
                    throw ({
                        res: FileResult.FileExists,
                        description: e.stack
                    })
                default:
                    throw ({
                        res: FileResult.Unknown,
                        description: "Unknown error occurred"
                    })
            }
        }
    }

    interface CopyInstance {
        progress?: (p: ProgressData)=>void, 
        onError?: (err: any)=>void
        jobs: CopyJob[]
    }

    interface CopyJob {
        move: boolean, 
        source: string, 
        size?: number
        targetDir: string
    }

    var copyInstance: CopyInstance = {jobs: []}

    const addCopyOrMove = (copyJob: CopyJob, progress: (p: ProgressData)=>void, onError: (err: any)=>void) => {
        copyInstance.onError = onError
        copyInstance.progress = progress
        copyInstance.jobs.push(copyJob)

        // TODO: retrieve size of folder or file

        if (copyInstance.jobs.length == 1)
            setInterval(() => processCopyJobs())
    }

    const processCopyJobs = async () => {
        while (copyInstance.jobs.length > 0) {
            const job = copyInstance.jobs.shift()
            await copyOrMove(job)
        }
        // TODO: call end! or progress 100%
    }

    const copyOrMove = (copyJob: CopyJob) => 
        new Promise<void>(res => {
            let progress = copyInstance.progress
            const process = spawn(copyJob.move ? 'mv' : 'cp' ,[copyJob.source, copyJob.targetDir])    
            const progressId = setInterval(async () => {
                const progressResult = await runCmd(`progress -p ${process.pid}`)
                const percentage = 
                    progressResult.split('\n')
                    .filter(n => n.includes('% ('))
                    .map(n => {
                        const words = n.split('%').map(n => n.trim())
                        return words[0]
                    })[0]
                    if (progress) {
                        // todo: get size of file, add it
                        progress({progress: parseFloat(percentage)})
                    }
                if (percentage == "100.0") {
                    clearInterval(progressId)
                    progress = null
                }
            }, 1000)
            process.once("exit", () => {
                if (progress) {
                    // todo: if not size of file, get it and add it
                    progress({progress: 100.0})
                }
                clearInterval(progressId)
                res()
            })
        })

    interface ProgressData {
        progress: number
    }

    // TODO: -2: get file size of 10_000 files sync 
    // TODO: -1: get file size of 10_000 files  
    // TODO: 0. always determine or add complete size (from file info or determine)
    // TODO: 1. copy one large file
    // TODO: 2. copy one small file (get file sie)
    // TODO: 3. copy 6 large files
    // TODO: 4. copy 6 small files

    // TODO: recursively unpack sourceDir, get file size from du
    const copy = (source: string, targetDir: string, progress: (p: ProgressData)=>void, onError: (err: any)=>void) => 
        addCopyOrMove({move: false, source, targetDir}, progress, onError)
    const move = (source: string, targetDir: string, progress: (p: ProgressData)=>void, onError: (err: any)=>void) => 
        addCopyOrMove({move: true, source, targetDir}, progress, onError)

    exports.getDrives = getDrives            
    exports.getIcon = getIcon
    exports.trash = trash
    exports.createFolder = createFolder
    exports.copy = copy
    exports.move = move
} else {
    exports.getDrives = inner.getDrives
    exports.getIcon = inner.getIcon
    // TODO: trash (windows)
    // TODO: createFolder (windows)
    // TODO: copy (windows)
    // TODO: move (windows)
}
