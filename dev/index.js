console.log("Starting test")
const addon = require('bindings')('filesystem-utilities')

;(async () => {
    const drives = await addon.getDrives()
    console.log("drives", drives)

    const files = await addon.getFiles("build")
    console.log("files", files)

    await runProfileTests()
})()

async function runProfileTests() {

    const start = process.hrtime.bigint()
    const files = await addon.getFiles("/media/Speicher/bilder/2017/Abu Dabbab")
    const sortedFiles = files.sort((a, b) => -a.name.localeCompare(b.name))
    let end = process.hrtime.bigint()
    console.info(`Execution time getFiles: ${((end - start)/ BigInt(1000000.0))} ms`)
    console.log("files", files.length)
}

// TODO: Windows version without ..
// TODO: check timing 4000 files
// TODO: sort by name

