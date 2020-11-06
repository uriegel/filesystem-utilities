const inner = require("./build/Release/filesystem-utilities")

function machnix() {
    return "nixgemacht"
}

exports.machnix = machnix
exports.getFiles = inner.getFiles