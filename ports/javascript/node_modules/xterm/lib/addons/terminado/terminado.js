"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function terminadoAttach(term, socket, bidirectional, buffered) {
    var addonTerminal = term;
    bidirectional = (typeof bidirectional === 'undefined') ? true : bidirectional;
    addonTerminal.__socket = socket;
    addonTerminal.__flushBuffer = function () {
        addonTerminal.write(addonTerminal.__attachSocketBuffer);
        addonTerminal.__attachSocketBuffer = null;
    };
    addonTerminal.__pushToBuffer = function (data) {
        if (addonTerminal.__attachSocketBuffer) {
            addonTerminal.__attachSocketBuffer += data;
        }
        else {
            addonTerminal.__attachSocketBuffer = data;
            setTimeout(addonTerminal.__flushBuffer, 10);
        }
    };
    addonTerminal.__getMessage = function (ev) {
        var data = JSON.parse(ev.data);
        if (data[0] === 'stdout') {
            if (buffered) {
                addonTerminal.__pushToBuffer(data[1]);
            }
            else {
                addonTerminal.write(data[1]);
            }
        }
    };
    addonTerminal.__sendData = function (data) {
        socket.send(JSON.stringify(['stdin', data]));
    };
    addonTerminal.__setSize = function (size) {
        socket.send(JSON.stringify(['set_size', size.rows, size.cols]));
    };
    socket.addEventListener('message', addonTerminal.__getMessage);
    if (bidirectional) {
        addonTerminal._core.register(addonTerminal.onData(addonTerminal.__sendData));
    }
    addonTerminal._core.register(addonTerminal.onResize(addonTerminal.__setSize));
    socket.addEventListener('close', function () { return terminadoDetach(addonTerminal, socket); });
    socket.addEventListener('error', function () { return terminadoDetach(addonTerminal, socket); });
}
exports.terminadoAttach = terminadoAttach;
function terminadoDetach(term, socket) {
    var addonTerminal = term;
    addonTerminal.__dataListener.dispose();
    addonTerminal.__dataListener = undefined;
    socket = (typeof socket === 'undefined') ? addonTerminal.__socket : socket;
    if (socket) {
        socket.removeEventListener('message', addonTerminal.__getMessage);
    }
    delete addonTerminal.__socket;
}
exports.terminadoDetach = terminadoDetach;
function apply(terminalConstructor) {
    terminalConstructor.prototype.terminadoAttach = function (socket, bidirectional, buffered) {
        return terminadoAttach(this, socket, bidirectional, buffered);
    };
    terminalConstructor.prototype.terminadoDetach = function (socket) {
        return terminadoDetach(this, socket);
    };
}
exports.apply = apply;
//# sourceMappingURL=terminado.js.map