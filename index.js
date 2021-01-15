"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.getFileVersion = exports.getExifDate = exports.getFiles = exports.FileResult = void 0;
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
})(FileResult = exports.FileResult || (exports.FileResult = {}));
const requireAddon = () => {
    try {
        return require("./build/Release/filesystem-utilities");
    }
    catch (err) {
        return require('bindings')('filesystem-utilities');
    }
};
const inner = requireAddon();
exports.getFiles = inner.getFiles;
exports.getExifDate = inner.getExifDate;
exports.getFileVersion = inner.getFileVersion;
if (process.platform == "linux") {
    const runCmd = (cmd) => new Promise(res => exec(cmd, (_, stdout) => res(stdout)));
    const getDrives = async () => {
        const drivesString = await runCmd('lsblk --bytes --output SIZE,NAME,LABEL,MOUNTPOINT,FSTYPE');
        const driveStrings = drivesString.split("\n");
        const columnsPositions = (() => {
            const title = driveStrings[0];
            const getPart = (key) => title.indexOf(key);
            return [
                0,
                getPart("NAME"),
                getPart("LABEL"),
                getPart("MOUNT"),
                getPart("FSTYPE")
            ];
        })();
        const takeOr = (text, alt) => text ? text : alt;
        const constructDrives = (driveString) => {
            const getString = (pos1, pos2) => driveString.substring(columnsPositions[pos1], columnsPositions[pos2]).trim();
            const trimName = (name) => name.length > 2 && name[1] == '─'
                ? name.substring(2)
                : name;
            const mount = getString(3, 4);
            return {
                name: takeOr(getString(2, 3), mount),
                description: trimName(getString(1, 2)),
                type: 1,
                mountPoint: mount,
                driveType: driveString.substring(columnsPositions[4]).trim(),
                size: parseInt(getString(0, 1), 10)
            };
        };
        return driveStrings
            .slice(1)
            .map(constructDrives)
            .filter(n => n.mountPoint && !n.mountPoint.startsWith("/snap"));
    };
    const getIcon = (ext) => new Promise((res, rej) => {
        const process = spawn('python3', [path.join(__dirname, "getIcon.py"), ext]);
        process.stdout.on('data', (data) => {
            const icon = data.toString('utf8').trim();
            res(icon);
        });
        process.stderr.on('data', (data) => rej(data.toString('utf8').trim()));
    });
    const trash = async (pathes) => {
        if (Array.isArray(pathes))
            for (const path of pathes)
                await trashOneFile(path);
        else
            await trashOneFile(pathes);
    };
    const trashOneFile = (file) => new Promise((res, rej) => {
        const process = spawn('python3', [path.join(__dirname, "delete.py"), file]);
        process.stdout.on('data', () => res());
        process.stderr.on('data', (data) => {
            const z = data.toString('utf8').trim();
            const err = Number.parseInt(z);
            switch (err) {
                case 1:
                    rej({
                        res: FileResult.FileNotFound,
                        description: "file not found"
                    });
                    break;
                case 15:
                    rej({
                        res: FileResult.AccessDenied,
                        description: "Access denied"
                    });
                    break;
                default:
                    rej({
                        res: FileResult.Unknown,
                        description: "Unknown error occurred"
                    });
                    break;
            }
        });
    });
    const createFolder = async (path) => {
        try {
            await fsa.mkdir(path);
        }
        catch (e) {
            switch (e.errno) {
                case -13:
                    throw ({
                        res: FileResult.AccessDenied,
                        description: e.stack
                    });
                case -17:
                    throw ({
                        res: FileResult.FileExists,
                        description: e.stack
                    });
                default:
                    throw ({
                        res: FileResult.Unknown,
                        description: "Unknown error occurred"
                    });
            }
        }
    };
    var copyInstance = { jobs: [] };
    const addCopyOrMove = (copyJob, progress, onError) => {
        copyInstance.onError = onError;
        copyInstance.progress = progress;
        copyInstance.jobs.push(copyJob);
        // TODO: retrieve size of folder or file
        if (copyInstance.jobs.length == 1)
            setInterval(() => processCopyJobs());
    };
    const processCopyJobs = async () => {
        while (copyInstance.jobs.length > 0) {
            const job = copyInstance.jobs.shift();
            await copyOrMove(job);
        }
        // TODO: call end! or progress 100%
    };
    const copyOrMove = (copyJob) => new Promise(res => {
        let progress = copyInstance.progress;
        const process = spawn(copyJob.move ? 'mv' : 'cp', [copyJob.source, copyJob.targetDir]);
        const getProgress = async () => {
            const progressResult = await runCmd(`progress -p ${process.pid}`);
            const percentage = progressResult.split('\n')
                .filter(n => n.includes('% ('))
                .map(n => {
                const words = n.split('%').map(n => n.trim());
                return words[0];
            })[0];
            if (progress) {
                progress({ progress: parseFloat(percentage) });
            }
            if (percentage == "100.0")
                progress = null;
        };
        const progressId = setInterval(getProgress, 1000);
        process.once("exit", async () => {
            clearInterval(progressId);
            if (progress)
                await getProgress();
            res();
        });
    });
    const copy = (source, targetDir, progress, onError) => addCopyOrMove({ move: false, source, targetDir }, progress, onError);
    const move = (source, targetDir, progress, onError) => addCopyOrMove({ move: true, source, targetDir }, progress, onError);
    exports.getDrives = getDrives;
    exports.getIcon = getIcon;
    exports.trash = trash;
    exports.createFolder = createFolder;
    exports.copy = copy;
    exports.move = move;
}
else {
    exports.getDrives = inner.getDrives;
    exports.getIcon = inner.getIcon;
    // TODO: trash (windows)
    // TODO: createFolder (windows)
    // TODO: copy (windows)
    // TODO: move (windows)
}
//# sourceMappingURL=index.js.map