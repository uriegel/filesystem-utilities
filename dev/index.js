console.log("Starting test")
const addon = require('../index')

const wait = () => new Promise(res => {})

;(async () => {
    const drives = await addon.getDrives()
    console.log("drives", drives)

    const iconPath = ".js"
    //const iconPath = "C:\\Windows\\regedit.exe"
    const icon = await addon.getIcon(iconPath)

    //await addon.copy("/home/uwe/Videos//home/uwe/Videos/essen.mp4", ".", progress => console.log(progress))
    // await addon.copy("/home/uwe/Videos/Nur die Sonne war Zeuge.mp4", ".", progress => console.log(progress))
    // return
    try {
        await addon.trash("/home/uwe/Projekte/eintest")
    } catch (err) {
        console.log(err)
    }
    await addon.createFolder("/home/uwe/Projekte/eintest")

    addon.copy("/home/uwe/Videos/Nur die Sonne war Zeuge.mp4", "/home/uwe/Projekte/eintest", p => console.log(p))
    await wait()

    try {
        await addon.createFolder("/home/uwe/Projekte/eintest")
    } catch (err) {
        console.log(err)
    }
    try {
        await addon.createFolder("/home/affe")
    } catch (err) {
        console.log(err)
    }
    await addon.trash("/home/uwe/Projekte/eintest")

    try {
        await addon.trash("/home/uwe/Projekte/eintest")
    } catch (err) {
        console.log(err)
    }

    try {
        await addon.trash("/mnt")
    } catch (err) {
        console.log(err)
    }

    const files = await addon.getFiles("build")
    console.log("files", files)

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


