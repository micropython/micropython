
const mpy = require('./build/micropython.js');

module.exports.run = function(code) {
    mpy.ccall('mp_js_run', 'null', ['string'], [code]);
}

module.exports.init = function(opts) {
    var import_wrapped = function(file_ptr) {
        var filename = mpy.Pointer_stringify(file_ptr);

        if (!opts.import_stat != null) {
            return opts.import_stat(filename);
        } else {
            return 0;
        }
    }

    var read_wrapped = function(file_ptr) {
        var filename = mpy.Pointer_stringify(file_ptr);

        if (opts.read_file != null) {
            return opts.read_file(filename);
        } else {
            return 0;
        }
    }

    mpy.ccall('mp_js_init', 'null', ['int', 'int'], [mpy.Runtime.addFunction(import_wrapped), mpy.Runtime.addFunction(read_wrapped)]);
}

module.exports.Module = mpy;
