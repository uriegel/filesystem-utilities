console.log("Starting test")
const addon = require('../index')

;(async () => {
    const drives = await addon.getDrives()
    console.log("drives", drives)

    const files = await addon.getFiles("build")
    console.log("files", files)

    // Linux:
    const icon = await addon.getIcon(".js")

    await runProfileTests()
})()

async function runProfileTests() {

    const start = process.hrtime.bigint()
    //const path = "/media/Speicher/bilder/2017/Abu Dabbab"
    const path = "c:\\windows\\system32"
    const files = await addon.getFiles(path)
    const sortedFiles = files.sort((a, b) => -a.name.localeCompare(b.name))
    let end = process.hrtime.bigint()
    console.info(`Execution time getFiles: ${((end - start)/ BigInt(1000000.0))} ms`)
    console.log("files", files.length)
}


