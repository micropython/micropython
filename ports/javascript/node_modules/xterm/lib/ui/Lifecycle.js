"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function addDisposableDomListener(node, type, handler, useCapture) {
    node.addEventListener(type, handler, useCapture);
    return {
        dispose: function () {
            if (!handler) {
                return;
            }
            node.removeEventListener(type, handler, useCapture);
        }
    };
}
exports.addDisposableDomListener = addDisposableDomListener;
//# sourceMappingURL=Lifecycle.js.map