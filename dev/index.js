console.log("Starting test")
const addon = require('../index')

const wait = time => new Promise(res => {
    setTimeout(() => res(), time)
})

;(async () => {

    const start = process.hrtime.bigint()    
    for (let i = 0; i < 1000; i++) {
        const size = addon.getFileSizeSync("/home/uwe/Bilder/Fotos/2015/Handy.jpg")
    }
    let end = process.hrtime.bigint()
    console.info(`Execution time getFileSizeSync: ${((end - start)/ BigInt(1000.0))} mys`)

    const start2 = process.hrtime.bigint()    
    for (let i = 0; i < 1000; i++) {
        const size = await addon.getFileSize("/home/uwe/Bilder/Fotos/2015/Handy.jpg")
    }
    let end2 = process.hrtime.bigint()
    console.info(`Execution time getFileSize: ${((end2 - start2)/ BigInt(1000.0))} mys`)

    const drives = await addon.getDrives()
    console.log("drives", drives)

    const iconPath = ".js"
    //const iconPath = "C:\\Windows\\regedit.exe"
    const icon = await addon.getIcon(iconPath)

    try {
        await addon.trash("/home/uwe/Projekte/eintest")
    } catch (err) {
        console.log(err)
    }
    await addon.createFolder("/home/uwe/Projekte/eintest")

    const progress = p => console.log(`${p.name} - ${p.size} - ${p.progress} - ${p.totalProgress}`)

    const copy1 = async () => {
        await addon.copy([ 
            "/home/uwe/Videos/Mandela.mp4",
            "/home/uwe/Videos/Nur die Sonne war Zeuge.mp4" ], "/home/uwe/Projekte/eintest", progress)
        console.log("Fertig 1")
    }

    const copy2 = async () => {
        await addon.copy([ "/home/uwe/Videos/Die Braut trug schwarz.mkv" ], "/home/uwe/Projekte/eintest", progress)
        console.log("Fertig 2")
    }

    copy1()
    await wait(5000)
    copy2()
    // try {
    //     await addon.createFolder("/home/uwe/Projekte/eintest")
    // } catch (err) {
    //     console.log(err)
    // }
    // try {
    //     await addon.createFolder("/home/affe")
    // } catch (err) {
    //     console.log(err)
    // }
    // await addon.trash("/home/uwe/Projekte/eintest")

    // try {
    //     await addon.trash("/home/uwe/Projekte/eintest")
    // } catch (err) {
    //     console.log(err)
    // }

    // try {
    //     await addon.trash("/mnt")
    // } catch (err) {
    //     console.log(err)
    // }

    // const files = await addon.getFiles("build")
    // console.log("files", files)

    // const imgpath = "/media/Speicher/bilder/2020/Canon/IMG_0110.JPG"
    // //const imgpath = "F:\\Bilder\\Tina\\2019\\12\\IMG_20191201_152308.jpg"
    // const exifDate = await addon.getExifDate(imgpath)

    // await runProfileTests()
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


