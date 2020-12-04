console.log("Starting test")
const addon = require('../index')

;(async () => {
    const drives = await addon.getDrives()
    console.log("drives", drives)

    try {
        await addon.trash("/etc/affe.json")
    } catch (err) {
        switch (err) {
            case 14:
                console.log("Permission denied")
                break;
            case 1:
                console.log("File not found")
                break;
        }
    }
    

    const files = await addon.getFiles("build")
    console.log("files", files)

    const iconPath = ".js"
    //const iconPath = "C:\\Windows\\regedit.exe"
    const icon = await addon.getIcon(iconPath)

    const imgpath = "/media/Speicher/bilder/2020/Canon/IMG_0110.JPG"
    //const imgpath = "F:\\Bilder\\Tina\\2019\\12\\IMG_20191201_152308.jpg"
    const exifDate = await addon.getExifDate(imgpath)

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


