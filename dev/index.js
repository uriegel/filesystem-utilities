console.log("Starting test")
const addon = require('bindings')('filesystem-utilities')

;(async () => {
    const drives = await addon.getDrives()
    console.log("drives", drives)
})()

