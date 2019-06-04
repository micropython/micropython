"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function toggleFullScreen(term, fullscreen) {
    var fn;
    if (typeof fullscreen === 'undefined') {
        fn = (term.element.classList.contains('fullscreen')) ?
            term.element.classList.remove : term.element.classList.add;
    }
    else if (!fullscreen) {
        fn = term.element.classList.remove;
    }
    else {
        fn = term.element.classList.add;
    }
    fn = fn.bind(term.element.classList);
    fn('fullscreen');
}
exports.toggleFullScreen = toggleFullScreen;
function apply(terminalConstructor) {
    terminalConstructor.prototype.toggleFullScreen = function (fullscreen) {
        toggleFullScreen(this, fullscreen);
    };
}
exports.apply = apply;
//# sourceMappingURL=fullscreen.js.map