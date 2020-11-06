const exec = require("child_process").exec
const process = require("process")

function requireAddon() {
    try {
        return require("./build/Release/filesystem-utilities")        
    } catch (err) {
        return require('bindings')('filesystem-utilities')
    }
}

const inner = requireAddon()
exports.getFiles = inner.getFiles

if (process.platform == "linux") {

    function runCmd(cmd) {
        return new Promise(res => 
            exec(cmd, (_, stdout) => res(stdout))
        )
    }

    async function getDrives() {
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
    
    exports.getDrives = getDrives            
}
else
    exports.getDrives = inner.getDrives


