"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var zmodem;
function zmodemAttach(ws, opts) {
    if (opts === void 0) { opts = {}; }
    var term = this;
    var senderFunc = function (octets) { return ws.send(new Uint8Array(octets)); };
    var zsentry;
    function shouldWrite() {
        return !!zsentry.get_confirmed_session() || !opts.noTerminalWriteOutsideSession;
    }
    zsentry = new zmodem.Sentry({
        to_terminal: function (octets) {
            if (shouldWrite()) {
                term.write(String.fromCharCode.apply(String, octets));
            }
        },
        sender: senderFunc,
        on_retract: function () { return term.emit('zmodemRetract'); },
        on_detect: function (detection) { return term.emit('zmodemDetect', detection); }
    });
    function handleWSMessage(evt) {
        if (typeof evt.data === 'string') {
            if (shouldWrite()) {
                term.write(evt.data);
            }
        }
        else {
            zsentry.consume(evt.data);
        }
    }
    ws.binaryType = 'arraybuffer';
    ws.addEventListener('message', handleWSMessage);
}
function apply(terminalConstructor) {
    zmodem = (typeof window === 'object') ? window.Zmodem : { Browser: null };
    terminalConstructor.prototype.zmodemAttach = zmodemAttach;
    terminalConstructor.prototype.zmodemBrowser = zmodem.Browser;
}
exports.apply = apply;
//# sourceMappingURL=zmodem.js.map