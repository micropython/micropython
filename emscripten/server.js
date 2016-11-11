#!/usr/bin/env node

var repl = require('repl');
var mpy = require('./main.js');
var argv = require('minimist')(process.argv.slice(2));
var fs = require('fs');

function mpy_eval(cmd, context, filename, callback) {
      callback(null, mpy.run(cmd)); 
}

mpy.init({
    import_stat: function(filename) {
        try {
            var stat = fs.statSync(filename);

            if (stat.isDirectory()) {
                return 1;
            } else if (stat.isFile()) {
                return 0;
            }
        } catch (e) {
            return -1;
        }

        return -1;
    },
    read_file: function(filename) {
        var content = fs.readFileSync(filename, 'utf-8');
        var ptr = mpy.Module.allocate(mpy.Module.intArrayFromString(content), 'i8', mpy.Module.ALLOC_NORMAL);

        return ptr;
    }
});

if (argv._.length == 1) {
    var code = fs.readFileSync(argv._[0], "utf-8");
    mpy.run(code);
} else if (argv.c) {
    var code = argv.c;
    mpy.run(code);
} else if (argv.m) {
    var code = 'import ' + argv.m;
    mpy.run(code);
} else if (argv._.length == 0) {
    console.log("MicroPython on asmjs with emscripten");
    console.log("Type \"help()\" for more information.");
    repl.start({prompt: '> ', eval: mpy_eval});
}

