#!/usr/bin/env node

var repl = require('repl');
var mpy = require('./main.js');

function mpy_eval(cmd, context, filename, callback) {
      callback(null, mpy.run(cmd)); 
}


console.log("MicroPython on asmjs with emscripten");
console.log("Type \"help()\" for more information.");

mpy.init();
repl.start({prompt: '> ', eval: mpy_eval});

