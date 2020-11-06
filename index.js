// TODO: if debugging this file, require with binding 
// try require catch require debug
const inner = require("./build/Release/filesystem-utilities")

// TODO: recognize os
// import * as process from "process"
// process.platform == "linux"
function machnix() {
    return "nixgemacht"
}

exports.machnix = machnix
exports.getFiles = inner.getFiles