"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function stringFromCodePoint(codePoint) {
    if (codePoint > 0xFFFF) {
        codePoint -= 0x10000;
        return String.fromCharCode((codePoint >> 10) + 0xD800) + String.fromCharCode((codePoint % 0x400) + 0xDC00);
    }
    return String.fromCharCode(codePoint);
}
exports.stringFromCodePoint = stringFromCodePoint;
function utf32ToString(data, start, end) {
    if (start === void 0) { start = 0; }
    if (end === void 0) { end = data.length; }
    var result = '';
    for (var i = start; i < end; ++i) {
        var codepoint = data[i];
        if (codepoint > 0xFFFF) {
            codepoint -= 0x10000;
            result += String.fromCharCode((codepoint >> 10) + 0xD800) + String.fromCharCode((codepoint % 0x400) + 0xDC00);
        }
        else {
            result += String.fromCharCode(codepoint);
        }
    }
    return result;
}
exports.utf32ToString = utf32ToString;
var StringToUtf32 = (function () {
    function StringToUtf32() {
        this._interim = 0;
    }
    StringToUtf32.prototype.clear = function () {
        this._interim = 0;
    };
    StringToUtf32.prototype.decode = function (input, target) {
        var length = input.length;
        if (!length) {
            return 0;
        }
        var size = 0;
        var startPos = 0;
        if (this._interim) {
            var second = input.charCodeAt(startPos++);
            if (0xDC00 <= second && second <= 0xDFFF) {
                target[size++] = (this._interim - 0xD800) * 0x400 + second - 0xDC00 + 0x10000;
            }
            else {
                target[size++] = this._interim;
                target[size++] = second;
            }
            this._interim = 0;
        }
        for (var i = startPos; i < length; ++i) {
            var code = input.charCodeAt(i);
            if (0xD800 <= code && code <= 0xDBFF) {
                if (++i >= length) {
                    this._interim = code;
                    return size;
                }
                var second = input.charCodeAt(i);
                if (0xDC00 <= second && second <= 0xDFFF) {
                    target[size++] = (code - 0xD800) * 0x400 + second - 0xDC00 + 0x10000;
                }
                else {
                    target[size++] = code;
                    target[size++] = second;
                }
                continue;
            }
            target[size++] = code;
        }
        return size;
    };
    return StringToUtf32;
}());
exports.StringToUtf32 = StringToUtf32;
var Utf8ToUtf32 = (function () {
    function Utf8ToUtf32() {
        this.interim = new Uint8Array(3);
    }
    Utf8ToUtf32.prototype.clear = function () {
        this.interim.fill(0);
    };
    Utf8ToUtf32.prototype.decode = function (input, target) {
        var length = input.length;
        if (!length) {
            return 0;
        }
        var size = 0;
        var byte1;
        var byte2;
        var byte3;
        var byte4;
        var codepoint = 0;
        var startPos = 0;
        if (this.interim[0]) {
            var discardInterim = false;
            var cp = this.interim[0];
            cp &= ((((cp & 0xE0) === 0xC0)) ? 0x1F : (((cp & 0xF0) === 0xE0)) ? 0x0F : 0x07);
            var pos = 0;
            var tmp = void 0;
            while ((tmp = this.interim[++pos] & 0x3F) && pos < 4) {
                cp <<= 6;
                cp |= tmp;
            }
            var type = (((this.interim[0] & 0xE0) === 0xC0)) ? 2 : (((this.interim[0] & 0xF0) === 0xE0)) ? 3 : 4;
            var missing = type - pos;
            while (startPos < missing) {
                if (startPos >= length) {
                    return 0;
                }
                tmp = input[startPos++];
                if ((tmp & 0xC0) !== 0x80) {
                    startPos--;
                    discardInterim = true;
                    break;
                }
                else {
                    this.interim[pos++] = tmp;
                    cp <<= 6;
                    cp |= tmp & 0x3F;
                }
            }
            if (!discardInterim) {
                if (type === 2) {
                    if (cp < 0x80) {
                        startPos--;
                    }
                    else {
                        target[size++] = cp;
                    }
                }
                else if (type === 3) {
                    if (cp < 0x0800 || (cp >= 0xD800 && cp <= 0xDFFF)) {
                    }
                    else {
                        target[size++] = cp;
                    }
                }
                else {
                    if (codepoint < 0x010000 || codepoint > 0x10FFFF) {
                    }
                    else {
                        target[size++] = cp;
                    }
                }
            }
            this.interim.fill(0);
        }
        var fourStop = length - 4;
        var i = startPos;
        while (i < length) {
            while (i < fourStop
                && !((byte1 = input[i]) & 0x80)
                && !((byte2 = input[i + 1]) & 0x80)
                && !((byte3 = input[i + 2]) & 0x80)
                && !((byte4 = input[i + 3]) & 0x80)) {
                target[size++] = byte1;
                target[size++] = byte2;
                target[size++] = byte3;
                target[size++] = byte4;
                i += 4;
            }
            byte1 = input[i++];
            if (byte1 < 0x80) {
                target[size++] = byte1;
            }
            else if ((byte1 & 0xE0) === 0xC0) {
                if (i >= length) {
                    this.interim[0] = byte1;
                    return size;
                }
                byte2 = input[i++];
                if ((byte2 & 0xC0) !== 0x80) {
                    i--;
                    continue;
                }
                codepoint = (byte1 & 0x1F) << 6 | (byte2 & 0x3F);
                if (codepoint < 0x80) {
                    i--;
                    continue;
                }
                target[size++] = codepoint;
            }
            else if ((byte1 & 0xF0) === 0xE0) {
                if (i >= length) {
                    this.interim[0] = byte1;
                    return size;
                }
                byte2 = input[i++];
                if ((byte2 & 0xC0) !== 0x80) {
                    i--;
                    continue;
                }
                if (i >= length) {
                    this.interim[0] = byte1;
                    this.interim[1] = byte2;
                    return size;
                }
                byte3 = input[i++];
                if ((byte3 & 0xC0) !== 0x80) {
                    i--;
                    continue;
                }
                codepoint = (byte1 & 0x0F) << 12 | (byte2 & 0x3F) << 6 | (byte3 & 0x3F);
                if (codepoint < 0x0800 || (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
                    continue;
                }
                target[size++] = codepoint;
            }
            else if ((byte1 & 0xF8) === 0xF0) {
                if (i >= length) {
                    this.interim[0] = byte1;
                    return size;
                }
                byte2 = input[i++];
                if ((byte2 & 0xC0) !== 0x80) {
                    i--;
                    continue;
                }
                if (i >= length) {
                    this.interim[0] = byte1;
                    this.interim[1] = byte2;
                    return size;
                }
                byte3 = input[i++];
                if ((byte3 & 0xC0) !== 0x80) {
                    i--;
                    continue;
                }
                if (i >= length) {
                    this.interim[0] = byte1;
                    this.interim[1] = byte2;
                    this.interim[2] = byte3;
                    return size;
                }
                byte4 = input[i++];
                if ((byte4 & 0xC0) !== 0x80) {
                    i--;
                    continue;
                }
                codepoint = (byte1 & 0x07) << 18 | (byte2 & 0x3F) << 12 | (byte3 & 0x3F) << 6 | (byte4 & 0x3F);
                if (codepoint < 0x010000 || codepoint > 0x10FFFF) {
                    continue;
                }
                target[size++] = codepoint;
            }
            else {
            }
        }
        return size;
    };
    return Utf8ToUtf32;
}());
exports.Utf8ToUtf32 = Utf8ToUtf32;
//# sourceMappingURL=TextDecoder.js.map