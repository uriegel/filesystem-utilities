console.log("Starting test")
const addon = require('bindings')('filesystem-utilities')

;(async () => {
    const drives = await addon.getDrives()
    console.log("drives", drives)

    const files = await addon.getFiles("build")
    console.log("files", files)

})()

