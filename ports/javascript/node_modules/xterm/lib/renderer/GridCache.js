"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var GridCache = (function () {
    function GridCache() {
        this.cache = [];
    }
    GridCache.prototype.resize = function (width, height) {
        for (var x = 0; x < width; x++) {
            if (this.cache.length <= x) {
                this.cache.push([]);
            }
            for (var y = this.cache[x].length; y < height; y++) {
                this.cache[x].push(null);
            }
            this.cache[x].length = height;
        }
        this.cache.length = width;
    };
    GridCache.prototype.clear = function () {
        for (var x = 0; x < this.cache.length; x++) {
            for (var y = 0; y < this.cache[x].length; y++) {
                this.cache[x][y] = null;
            }
        }
    };
    return GridCache;
}());
exports.GridCache = GridCache;
//# sourceMappingURL=GridCache.js.map