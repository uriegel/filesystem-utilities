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
            await fsa.mkdir(path, { recursive: true })   
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
        size: number,
        processedSize: number,
        jobs: CopyJob[]
    }

    enum JobType {
        Copy,
        Move, 
        Delete
    }

    interface CopyJob {
        jobType: JobType, 
        source: string, 
        size?: number
        targetDir: string
    }

    interface ProgressData {
        name: string
        size: number
        progress: number
        totalSize: number
        totalProgress: number
    }

    const copyOrMove = async (move: boolean, sources: string[], targetDir: string, progress: (p: ProgressData)=>void) => {
        const copyInstance: CopyInstance = {size: 0, processedSize: 0, jobs: [] }

        const createCopyJob = (source: string, targetDir: string, size: number) => {
            const copyJob = {jobType: move ? JobType.Move : JobType.Copy, source, targetDir} as CopyJob
            copyJob.size = size
            copyInstance.size += copyJob.size
            copyInstance.progress = progress
            copyInstance.jobs.push(copyJob)
        }

        const fillCopyJob = async (source: string, target: string, firstCall?: boolean) => {
            const stat = await fsa.stat(source)
            if (!stat.isDirectory())
                createCopyJob(source, target, stat.size)
            else {
                const files = await fsa.readdir(source)
                for (let file of files) 
                    await fillCopyJob(path.join(source, file), path.join(target, path.basename(source)))                 
                if (firstCall) {
                    const deleteJob = {jobType: JobType.Delete, source, targetDir: ""} as CopyJob
                    copyInstance.jobs.push(deleteJob)
                }
            }
        }

        for (let source of sources) 
            await fillCopyJob(source, targetDir, move) 

        const copyOrMove = (copyJob: CopyJob) => 
            new Promise<void>(async (res, rej) => {
                let progress = (percentage: string) => {
                    if (copyInstance.progress) {
                        const recentProgress = parseFloat(percentage)
                        const progress = recentProgress / 100
                        copyInstance.progress({
                            name: copyJob.source,
                            progress,
                            size: copyJob.size,
                            totalProgress: (copyInstance.processedSize + (progress * copyJob.size)) / copyInstance.size,
                            totalSize: copyInstance.size
                        })
                    }
                }
                
                if (copyJob.jobType != JobType.Delete) {
                    if (!fs.existsSync(copyJob.targetDir))
                        try {
                            await createFolder(copyJob.targetDir)
                        } catch (err) { 
                            rej(err) 
                            return
                        }
                    const process = copyJob.jobType == JobType.Copy 
                                ? spawn('cp' ,[ "-p", copyJob.source, copyJob.targetDir])    
                                : spawn('mv' ,[ copyJob.source, copyJob.targetDir])     
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
                        if (percentage == "100.0") {
                            clearInterval(progressId)
                            progress = (p: string) => {}
                        }
                    }, 1000)
                    process.once("exit", async () => {
                        if (!process.exitCode) {
                            progress("100.0")
                            clearInterval(progressId)
                            copyInstance.processedSize += copyJob.size
                            res()
                        } else {
                            clearInterval(progressId)
                            rej({
                                res: FileResult.Unknown,
                                description: ""
                            })
                        }
                    })
                }
                else {
                    // delete root dir after moving all files
                    await fsa.rmdir(copyJob.source, {recursive: true})
                    res()
                }
            })

        const processCopyJobs = async () => {
            while (copyInstance.jobs.length > 0) {
                const job = copyInstance.jobs.shift()
                await copyOrMove(job)
            }
            copyInstance.progress = null
            copyInstance.size = 0
        }
        await processCopyJobs()
    } 

    const copy = (sources: string[], targetDir: string, progress: (p: ProgressData)=>void) => 
        copyOrMove (false, sources, targetDir, progress)
    const move = (sources: string[], targetDir: string, progress: (p: ProgressData)=>void) => 
        copyOrMove (true, sources, targetDir, progress)

    exports.getFiles = async path => {
        const items = await inner.getFiles(path)
        let dirs = items.filter(n => n.isDirectory)
        let files = items.filter(n => !n.isDirectory)
        return dirs.sort((a, b) => a.name.toLowerCase().localeCompare(b.name.toLowerCase()))
            .concat(files.sort((a, b) => a.name.toLowerCase().localeCompare(b.name.toLowerCase())))
    }
    
    exports.getDrives = getDrives            
    exports.getIcon = inner.getIcon
    exports.trash = trash
    exports.createFolder = createFolder
    exports.getFileSizeSync = inner.getFileSizeSync
    exports.getFileSize = inner.getFileSize
    exports.copy = copy
    exports.move = move
} else {

    const createFolder = async (path: string) => {
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
