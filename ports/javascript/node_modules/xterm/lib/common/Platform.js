"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var isNode = (typeof navigator === 'undefined') ? true : false;
var userAgent = (isNode) ? 'node' : navigator.userAgent;
var platform = (isNode) ? 'node' : navigator.platform;
exports.isFirefox = !!~userAgent.indexOf('Firefox');
exports.isSafari = /^((?!chrome|android).)*safari/i.test(userAgent);
exports.isMSIE = !!~userAgent.indexOf('MSIE') || !!~userAgent.indexOf('Trident');
exports.isMac = contains(['Macintosh', 'MacIntel', 'MacPPC', 'Mac68K'], platform);
exports.isIpad = platform === 'iPad';
exports.isIphone = platform === 'iPhone';
exports.isMSWindows = contains(['Windows', 'Win16', 'Win32', 'WinCE'], platform);
exports.isLinux = platform.indexOf('Linux') >= 0;
function contains(arr, el) {
    return arr.indexOf(el) >= 0;
}
//# sourceMappingURL=Platform.js.map