
console.log("Starting windows test")
const addon = require('../index')

const wait = time => new Promise(res => {
    setTimeout(() => res(), time)
})

;(async () => {
    const iconPath = "C:\\Windows\\regedit.exe"
    const icon = await addon.getIcon(iconPath)

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