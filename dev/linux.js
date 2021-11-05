console.log("Starting linux test")
const addon = require('../index')

const wait = time => new Promise(res => {
    setTimeout(() => res(), time)
})

;(async () => {
    await addon.copyFile("/home/uwe/Videos/raw/Goldeneye.mts", "/home/uwe/test/test.mts", (c, t) => console.log(`Progress js ${c}, ${t}`))


    
//     const fs = require('fs/promises')
//     const start3 = process.hrtime.bigint()    
//     for (let i = 0; i < 1000; i++) {
//         const size = await fs.stat("/home/uwe/Bilder/Fotos/2015/Handy.jpg")
//         const isDir = size.isDirectory()
//         const seize = size.size
//     }
//     let end3 = process.hrtime.bigint()
//     console.info(`Execution time getFileSize: ${((end3 - start3)/ BigInt(1000.0))} mys`)

//     const drives = await addon.getDrives()
//     console.log("drives", drives)

//     const files = await addon.getFiles("/home/uwe")
//     console.log("files", files)

//     const iconPath = ".js"
//     const icon = await addon.getIcon(iconPath)
//     const iconPath2 = "."
//     const icon2 = await addon.getIcon(iconPath2)


//     const dateFormat = Intl.DateTimeFormat("de-DE", {
//         year: "numeric",
//         month: "2-digit",
//         day: "2-digit",
//     })
//     const timeFormat = Intl.DateTimeFormat("de-DE", {
//         hour: "2-digit",
//         minute: "2-digit"
//     })
//     const formatDateTime = unixDate => {
//         if (!unixDate)
//             return ''
    
//         return dateFormat.format(unixDate) + " " + timeFormat.format(unixDate)  
//     }    
//     //const imgpath = "/media/Speicher/bilder/2020/Canon/IMG_0110.JPG"
//     const imgpath = "/home/uwe/Bilder/Fotos/2002/1/Bild309.jpg"
//     // //const imgpath = "F:\\Bilder\\Tina\\2019\\12\\IMG_20191201_152308.jpg"
//     const exifDate = await addon.getExifDate(imgpath)
//     console.log(new Date().getTime())
//     console.log(exifDate.getTime())



//     try {
//         await addon.trash("/home/uwe/Projekte/eintest")
//     } catch (err) {
//         console.log(err)
//     }
//     await addon.createFolder("/home/uwe/Projekte/eintest")

//     const progress = p => console.log(`${p.name} - ${p.size} - ${p.progress} - ${p.totalProgress}`)

//     const notCopy = async () => {
//         try {
//             await addon.copy([ "/home/uwe/GNUstep" ], "/etc", progress)
//             console.log("finished")
//         } catch (err) {
//             const errr = err
//             const affe = err
//         }
//     }
//     await notCopy()

//     const notCopy2 = async () => {
//         try {
//             await addon.copy([ "/home/uwe/Projekte/filesystem-utilities/tsconfig.json" ], "/etc", progress)
//             console.log("finished")
//         } catch (err) {
//             const errr = err
//             const affe = err
//         }
//     }
//     await notCopy2()

//     const copy = async () => {
// //        await addon.copy([ "/home/uwe/Bilder/Fotos/2017" ], "/home/uwe/Projekte/eintest", progress)
// //        await addon.copy([ "/home/uwe/GNUstep" ], "/home/uwe/Projekte/eintest", progress)
//         await addon.copy([ "/home/uwe/Projekte/commander/node_modules" ], "/home/uwe/Projekte/eintest")
//         console.log("finished")
//     }

//     const move = async () => {
//         await addon.move([ "/home/uwe/Projekte/eintest/GNUstep" ], "/home/uwe/Projekte/eintest/affe", progress)
//         console.log("finished move")
//     }

//     const copy1 = async () => {
//         await addon.copy([ 
//             "/home/uwe/Videos/Mandela.mp4",
//             "/home/uwe/Videos/Nur die Sonne war Zeuge.mp4" ], "/home/uwe/Projekte/eintest", progress)
//         console.log("finished 1")
//     }

//     const copy2 = async () => {
//         await addon.copy([ "/home/uwe/Videos/Die Braut trug schwarz.mkv" ], "/home/uwe/Projekte/eintest", progress)
//         console.log("finished 2")
//     }

//     await copy()
//     // await move()
//     // await wait(5000)
//     // copy1()
//     // await wait(5000)
//     // copy2()



//     // try {
//     //     await addon.createFolder("/home/uwe/Projekte/eintest")
//     // } catch (err) {
//     //     console.log(err)
//     // }
//     // try {
//     //     await addon.createFolder("/home/affe")
//     // } catch (err) {
//     //     console.log(err)
//     // }
//     // await addon.trash("/home/uwe/Projekte/eintest")

//     // try {
//     //     await addon.trash("/home/uwe/Projekte/eintest")
//     // } catch (err) {
//     //     console.log(err)
//     // }

//     // try {
//     //     await addon.trash("/mnt")
//     // } catch (err) {
//     //     console.log(err)
//     // }

//     // const files = await addon.getFiles("build")
//     // console.log("files", files)

//     // const imgpath = "/media/Speicher/bilder/2020/Canon/IMG_0110.JPG"
//     // //const imgpath = "F:\\Bilder\\Tina\\2019\\12\\IMG_20191201_152308.jpg"
//     // const exifDate = await addon.getExifDate(imgpath)

//     // await runProfileTests()
})()

// async function runProfileTests() {

//     const start = process.hrtime.bigint()
//     //const path = "/media/Speicher/bilder/2017/Abu Dabbab"
//     const path = "c:\\windows\\system32"
//     const files = await addon.getFiles(path)
//     const sortedFiles = files.sort((a, b) => -a.name.localeCompare(b.name))
//     let end = process.hrtime.bigint()
//     console.info(`Execution time getFiles: ${((end - start)/ BigInt(1000000.0))} ms`)
//     console.log("files", files.length)
// }


