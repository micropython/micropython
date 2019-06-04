"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var LRUMap = (function () {
    function LRUMap(capacity) {
        this.capacity = capacity;
        this._map = {};
        this._head = null;
        this._tail = null;
        this._nodePool = [];
        this.size = 0;
    }
    LRUMap.prototype._unlinkNode = function (node) {
        var prev = node.prev;
        var next = node.next;
        if (node === this._head) {
            this._head = next;
        }
        if (node === this._tail) {
            this._tail = prev;
        }
        if (prev !== null) {
            prev.next = next;
        }
        if (next !== null) {
            next.prev = prev;
        }
    };
    LRUMap.prototype._appendNode = function (node) {
        var tail = this._tail;
        if (tail !== null) {
            tail.next = node;
        }
        node.prev = tail;
        node.next = null;
        this._tail = node;
        if (this._head === null) {
            this._head = node;
        }
    };
    LRUMap.prototype.prealloc = function (count) {
        var nodePool = this._nodePool;
        for (var i = 0; i < count; i++) {
            nodePool.push({
                prev: null,
                next: null,
                key: null,
                value: null
            });
        }
    };
    LRUMap.prototype.get = function (key) {
        var node = this._map[key];
        if (node !== undefined) {
            this._unlinkNode(node);
            this._appendNode(node);
            return node.value;
        }
        return null;
    };
    LRUMap.prototype.peekValue = function (key) {
        var node = this._map[key];
        if (node !== undefined) {
            return node.value;
        }
        return null;
    };
    LRUMap.prototype.peek = function () {
        var head = this._head;
        return head === null ? null : head.value;
    };
    LRUMap.prototype.set = function (key, value) {
        var node = this._map[key];
        if (node !== undefined) {
            node = this._map[key];
            this._unlinkNode(node);
            node.value = value;
        }
        else if (this.size >= this.capacity) {
            node = this._head;
            this._unlinkNode(node);
            delete this._map[node.key];
            node.key = key;
            node.value = value;
            this._map[key] = node;
        }
        else {
            var nodePool = this._nodePool;
            if (nodePool.length > 0) {
                node = nodePool.pop();
                node.key = key;
                node.value = value;
            }
            else {
                node = {
                    prev: null,
                    next: null,
                    key: key,
                    value: value
                };
            }
            this._map[key] = node;
            this.size++;
        }
        this._appendNode(node);
    };
    return LRUMap;
}());
exports.default = LRUMap;
//# sourceMappingURL=LRUMap.js.map