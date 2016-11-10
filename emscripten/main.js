
const repl = require('repl');
const mpy = require('./build/firmware.js');

function mpy_eval(cmd, context, filename, callback) {
      callback(null, mpy.ccall('mp_js_run', 'null', ['string'], [cmd]));
}


console.log("MicroPython on asmjs with emscripten");
console.log("Type \"help()\" for more information.");

mpy.ccall('mp_js_init', 'null', [], []);
repl.start({prompt: '> ', eval: mpy_eval});

