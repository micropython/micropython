"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.DEFAULT_BELL_SOUND = 'data:audio/wav;base64,UklGRigBAABXQVZFZm10IBAAAAABAAEARKwAAIhYAQACABAAZGF0YQQBAADpAFgCwAMlBZoG/wdmCcoKRAypDQ8PbRDBEQQTOxRtFYcWlBePGIUZXhoiG88bcBz7HHIdzh0WHlMeZx51HmkeUx4WHs8dah0AHXwc3hs9G4saxRnyGBIYGBcQFv8U4RPAEoYRQBACD70NWwwHC6gJOwjWBloF7gOBAhABkf8b/qv8R/ve+Xf4Ife79W/0JfPZ8Z/wde9N7ijtE+wU6xvqM+lb6H7nw+YX5mrlxuQz5Mzje+Ma49fioeKD4nXiYeJy4pHitOL04j/jn+MN5IPkFOWs5U3mDefM55/ogOl36m7rdOyE7abuyu8D8Unyj/Pg9D/2qfcb+Yn6/vuK/Qj/lAAlAg==';
var SoundManager = (function () {
    function SoundManager(_terminal) {
        this._terminal = _terminal;
    }
    Object.defineProperty(SoundManager, "audioContext", {
        get: function () {
            if (!SoundManager._audioContext) {
                var audioContextCtor = window.AudioContext || window.webkitAudioContext;
                if (!audioContextCtor) {
                    console.warn('Web Audio API is not supported by this browser. Consider upgrading to the latest version');
                    return null;
                }
                SoundManager._audioContext = new audioContextCtor();
            }
            return SoundManager._audioContext;
        },
        enumerable: true,
        configurable: true
    });
    SoundManager.prototype.playBellSound = function () {
        var ctx = SoundManager.audioContext;
        if (!ctx) {
            return;
        }
        var bellAudioSource = ctx.createBufferSource();
        ctx.decodeAudioData(this._base64ToArrayBuffer(this._removeMimeType(this._terminal.options.bellSound)), function (buffer) {
            bellAudioSource.buffer = buffer;
            bellAudioSource.connect(ctx.destination);
            bellAudioSource.start(0);
        });
    };
    SoundManager.prototype._base64ToArrayBuffer = function (base64) {
        var binaryString = window.atob(base64);
        var len = binaryString.length;
        var bytes = new Uint8Array(len);
        for (var i = 0; i < len; i++) {
            bytes[i] = binaryString.charCodeAt(i);
        }
        return bytes.buffer;
    };
    SoundManager.prototype._removeMimeType = function (dataURI) {
        var splitUri = dataURI.split(',');
        return splitUri[1];
    };
    return SoundManager;
}());
exports.SoundManager = SoundManager;
//# sourceMappingURL=SoundManager.js.map