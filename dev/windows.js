
console.log("Starting windows test")
const addon = require('../index')

const wait = time => new Promise(res => {
    setTimeout(() => res(), time)
})

;(async () => {
    const iconPath = "C:\\Windows\\regedit.exe"
    const icon = await addon.getIcon(iconPath)

    try {
        await addon.createFolder("C:\\Users\\uwe\\Desktop\\Ordner")
    } catch (err) {
        console.log(err)
    }
    try {
        await addon.createFolder("C:\\Program Files\\Ordner")
    } catch (err) {
        console.log(err)
    }

})()