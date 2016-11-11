
const mpy = require('./build/firmware.js');

module.exports.run = function(code) {
    mpy.ccall('mp_js_run', 'null', ['string'], [code]);
}

module.exports.init = function() {
    mpy.ccall('mp_js_init', 'null', [], []);
}

