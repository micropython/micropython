/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

interface ILinkedListNode<T> {
  prev: ILinkedListNode<T>;
  next: ILinkedListNode<T>;
  key: number;
  value: T;
}

export default class LRUMap<T> {
  private _map: { [key: number]: ILinkedListNode<T> } = {};
  private _head: ILinkedListNode<T> = null;
  private _tail: ILinkedListNode<T> = null;
  private _nodePool: ILinkedListNode<T>[] = [];
  public size: number = 0;

  constructor(public capacity: number) { }

  private _unlinkNode(node: ILinkedListNode<T>): void {
    const prev = node.prev;
    const next = node.next;
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
  }

  private _appendNode(node: ILinkedListNode<T>): void {
    const tail = this._tail;
    if (tail !== null) {
      tail.next = node;
    }
    node.prev = tail;
    node.next = null;
    this._tail = node;
    if (this._head === null) {
      this._head = node;
    }
  }

  /**
   * Preallocate a bunch of linked-list nodes. Allocating these nodes ahead of time means that
   * they're more likely to live next to each other in memory, which seems to improve performance.
   *
   * Each empty object only consumes about 60 bytes of memory, so this is pretty cheap, even for
   * large maps.
   */
  public prealloc(count: number): void {
    const nodePool = this._nodePool;
    for (let i = 0; i < count; i++) {
      nodePool.push({
        prev: null,
        next: null,
        key: null,
        value: null
      });
    }
  }

  public get(key: number): T | null {
    // This is unsafe: We're assuming our keyspace doesn't overlap with Object.prototype. However,
    // it's faster than calling hasOwnProperty, and in our case, it would never overlap.
    const node = this._map[key];
    if (node !== undefined) {
      this._unlinkNode(node);
      this._appendNode(node);
      return node.value;
    }
    return null;
  }

  /**
   * Gets a value from a key without marking it as the most recently used item.
   */
  public peekValue(key: number): T | null {
    const node = this._map[key];
    if (node !== undefined) {
      return node.value;
    }
    return null;
  }

  public peek(): T | null {
    const head = this._head;
    return head === null ? null : head.value;
  }

  public set(key: number, value: T): void {
    // This is unsafe: See note above.
    let node = this._map[key];
    if (node !== undefined) {
      // already exists, we just need to mutate it and move it to the end of the list
      node = this._map[key];
      this._unlinkNode(node);
      node.value = value;
    } else if (this.size >= this.capacity) {
      // we're out of space: recycle the head node, move it to the tail
      node = this._head;
      this._unlinkNode(node);
      delete this._map[node.key];
      node.key = key;
      node.value = value;
      this._map[key] = node;
    } else {
      // make a new element
      const nodePool = this._nodePool;
      if (nodePool.length > 0) {
        // use a preallocated node if we can
        node = nodePool.pop();
        node.key = key;
        node.value = value;
      } else {
        node = {
          prev: null,
          next: null,
          key,
          value
        };
      }
      this._map[key] = node;
      this.size++;
    }
    this._appendNode(node);
  }
}
