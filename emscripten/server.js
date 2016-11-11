#!/usr/bin/env node

var repl = require('repl');
var mpy = require('./main.js');
var argv = require('minimist')(process.argv.slice(2));
var fs = require('fs');

function mpy_eval(cmd, context, filename, callback) {
      callback(null, mpy.run(cmd)); 
}

mpy.init();

if (argv._.length == 1) {
    var code = fs.readFileSync(argv._[0], "utf-8");
    mpy.run(code);
} else if (argv.c) {
    var code = argv.c;
    mpy.run(code);
} else if (argv.m) {
    // TODO: make object loading work properly with Emscripten
} else if (argv._.length == 0) {
    console.log("MicroPython on asmjs with emscripten");
    console.log("Type \"help()\" for more information.");
    repl.start({prompt: '> ', eval: mpy_eval});
}

