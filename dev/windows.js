
console.log("Starting windows test")
const addon = require('../index')

const wait = time => new Promise(res => {
    setTimeout(() => res(), time)
})


// addon.openFile("c:\\users\\urieg\\Downloads\\index.html")
// addon.openFileWith("c:\\users\\urieg\\Downloads\\index.html")
// addon.showFileProperties("c:\\users\\urieg\\Downloads\\index.html")

const delay = (span) => new Promise(res => setTimeout(res, span))

; (async () => {


        const handle = addon.observeWindowServices(items => {
            console.log("services", items)
        })
        await delay(20_000)
        handle.dispose()

        const icon1 = await addon.getIcon("//vme-test2024-1/c$/Program%20Files/caesar/CAEWebSrv/caesarWebServer.exe")

        const versions = await addon.getVersionInfos([ { path: "C:\\Windows\\HelpPane.exe", idx: 1 }, { path: "C:\\Windows\\win.ini", idx: 2 }, { path: "C:\\Windows\\regedit.exe", idx: 3 } ])



    try {

        try {
            await addon.copyFiles("C:\\Users\\urieg", "C:\\Users\\urieg\\source", ["Test"], { move: true })
        } catch (e) {
            console.log("error", e)
        }

        await addon.createFolder("c:\\Projekte\\Neuer Ordner");
        await addon.rename("c:\\Projekte", "Neuer Ordner", "umbenannt");
        await addon.trash("c:\\Projekte\\umbenannt");
        await addon.createFolder("c:\\windows\\Neuer Ordner");
        await addon.trash("c:\\windows\\Neuer Ordner");

        const version = await addon.getFileVersion("c:\\windows\\regedit.exe")
        console.log("version", version)
        await addon.copyFiles("D:\\Videos", "c:\\Projekte", ["HD\\Tribute12.mkv", "HD\\Tribute2.mkv"])
        //await addon.trash("c:\\Projekte\\hd");
        await addon.trash(["c:\\windows\\Tribute1.mkv", "c:\\windows\\Tribute2.mkv"])

        await addon.copyFiles("D:\\Videos", "c:\\Projekte", ["HD\\Tribute1.mkv", "HD\\Tribute2.mkv"])
        await addon.copyFiles("D:\\Videos", "c:\\Projekte", ["HD\\Tribute1.mkv", "HD\\Tribute2.mkv"])
        await addon.copyFiles("D:\\Videos", "c:\\Projekte", ["HD\\Tribute1.mkv", "HD\\Tribute2.mkv"], { overwrite: true})
        await addon.copyFiles("D:\\Videos\\HD", "c:\\windows", ["Tribute1.mkv", "Tribute2.mkv"])
    } catch (e) {
        console.log(e)
    }

    const drives = await addon.getDrives()
    console.log("drives", drives)

    const files = await addon.getFiles("C:\\users\\urieg", false)
    console.log("files", files)
    
    const filesHidden = await addon.getFiles("C:\\users\\urieg", true)
    console.log("filesHidden", filesHidden)

    try {
        const filesNoAccess = await addon.getFiles("C:\\MSOCache", true)
        console.log("filesHidden", filesHidden)
    } catch (e) {
        console.log("error", e)
    }

    try {
        const filesNo = await addon.getFilesAsync("C:\\Fantasie", true)
        console.log("filesNo", filesNo)
    } catch (e) {
        console.log("error", e)
    }

    const iconPath = "C:\\Windows\\regedit.exe"
    const icon = await addon.getIcon(iconPath)

    const gpx = await addon.getGpxTrackAsync("C:\\Users\\urieg\\2024-01-29-16-04.gpx")
    const gpxNo = await addon.getGpxTrackAsync("C:\\Users\\urieg\\2024-01-29-16-0No.gpx")

    try {
        await addon.createFolder("C:\\Users\\urieg\\OneDrive\\Desktop\\Ordner")
    } catch (err) {
        console.log(err)
    }
    try {
        await addon.copy(
            // ["D:\\Videos\\Ali.mkv"], 
            // ["C:\\Users\\urieg\\OneDrive\\Desktop\\Ordner\\Ali.mkv"])
            ["D:\\Videos\\Ali.mkv", "D:\\Videos\\Dinos\\Dino3.mpg"], 
            ["C:\\Users\\urieg\\OneDrive\\Desktop\\Ordner\\Ali.mkv", "C:\\Users\\urieg\\OneDrive\\Desktop\\Ordner\\Dinos\\Dino3.mpg"])
    } catch (err) {
        console.log(err)
    }
    try {
        //await addon.copy("C:\\Users\\urieg\\OneDrive\\Desktop\\Ordner", "C:\\Users\\urieg\\OneDrive\\Desktop", ["Ali.mkv"], true)
    } catch (err) {
        console.log(err)
    }
    try {
        await addon.copy("C:\\Users\\urieg\\OneDrive\\Desktop\\Ordner", "C:\\Users\\urieg\\OneDrive\\Desktop\\Abfall", true)
    } catch (err) {
        console.log(err)
    }
    try {
        await addon.trash(["C:\\Users\\urieg\\OneDrive\\Desktop\\Abfall"])
    } catch (err) {
        console.log(err)
    }

    try {
        await addon.createFolder("C:\\Program Files\\Ordner")
    } catch (err) {
        console.log(err)
    }
    try {
        await addon.trash(["C:\\Program Files\\Ordner"])
    } catch (err) {
        console.log(err)
    }
})()
