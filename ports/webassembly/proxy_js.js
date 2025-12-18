/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023-2024 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Number of static entries at the start of proxy_js_ref.
const PROXY_JS_REF_NUM_STATIC = 2;

// These constants should match the constants in proxy_c.c.

const PROXY_KIND_MP_EXCEPTION = -1;
const PROXY_KIND_MP_NULL = 0;
const PROXY_KIND_MP_NONE = 1;
const PROXY_KIND_MP_BOOL = 2;
const PROXY_KIND_MP_INT = 3;
const PROXY_KIND_MP_FLOAT = 4;
const PROXY_KIND_MP_STR = 5;
const PROXY_KIND_MP_CALLABLE = 6;
const PROXY_KIND_MP_GENERATOR = 7;
const PROXY_KIND_MP_OBJECT = 8;
const PROXY_KIND_MP_JSPROXY = 9;
const PROXY_KIND_MP_EXISTING = 10;

const PROXY_KIND_JS_UNDEFINED = 0;
const PROXY_KIND_JS_NULL = 1;
const PROXY_KIND_JS_BOOLEAN = 2;
const PROXY_KIND_JS_INTEGER = 3;
const PROXY_KIND_JS_DOUBLE = 4;
const PROXY_KIND_JS_STRING = 5;
const PROXY_KIND_JS_OBJECT_EXISTING = 6;
const PROXY_KIND_JS_OBJECT = 7;
const PROXY_KIND_JS_PYPROXY = 8;

class PythonError extends Error {
    constructor(exc_type, exc_details) {
        super(exc_details);
        this.name = "PythonError";
        this.type = exc_type;
    }
}

function proxy_js_init() {
    globalThis.proxy_js_ref = [globalThis, undefined];
    globalThis.proxy_js_ref_next = PROXY_JS_REF_NUM_STATIC;
    globalThis.proxy_js_ref_map = new Map();
    globalThis.proxy_js_ref_map.set(globalThis, 0);
    globalThis.proxy_js_map = new Map();
    globalThis.proxy_js_existing = [undefined];
    globalThis.pyProxyFinalizationRegistry = new FinalizationRegistry(
        (cRef) => {
            globalThis.proxy_js_map.delete(cRef);
            Module.ccall("proxy_c_free_obj", "null", ["number"], [cRef]);
        },
    );
}

// Check if the c_ref (Python proxy index) has a corresponding JavaScript-side PyProxy
// associated with it.  If so, take a concrete reference to this PyProxy from the WeakRef
// and put it in proxy_js_existing, to be referenced and reused by PROXY_KIND_MP_EXISTING.
function proxy_js_check_existing(c_ref) {
    const existing_obj = globalThis.proxy_js_map.get(c_ref)?.deref();
    if (existing_obj === undefined) {
        return -1;
    }

    // Search for a free slot in proxy_js_existing.
    for (let i = 0; i < globalThis.proxy_js_existing.length; ++i) {
        if (globalThis.proxy_js_existing[i] === undefined) {
            // Free slot found, put existing_obj here and return the index.
            globalThis.proxy_js_existing[i] = existing_obj;
            return i;
        }
    }

    // No free slot, so append to proxy_js_existing and return the new index.
    globalThis.proxy_js_existing.push(existing_obj);
    return globalThis.proxy_js_existing.length - 1;
}

// The `js_obj` argument cannot be `undefined`.
// Returns an integer reference to the given `js_obj`.
function proxy_js_add_obj(js_obj) {
    // Search for the first free slot in proxy_js_ref.
    while (proxy_js_ref_next < proxy_js_ref.length) {
        if (proxy_js_ref[proxy_js_ref_next] === undefined) {
            // Free slot found, reuse it.
            const id = proxy_js_ref_next;
            ++proxy_js_ref_next;
            proxy_js_ref[id] = js_obj;
            proxy_js_ref_map.set(js_obj, id);
            return id;
        }
        ++proxy_js_ref_next;
    }

    // No free slots, so grow proxy_js_ref by one (append at the end of the array).
    const id = proxy_js_ref.length;
    proxy_js_ref[id] = js_obj;
    proxy_js_ref_next = proxy_js_ref.length;
    proxy_js_ref_map.set(js_obj, id);
    return id;
}

function proxy_call_python(target, argumentsList) {
    let args = 0;

    // Strip trailing "undefined" arguments.
    while (
        argumentsList.length > 0 &&
        argumentsList[argumentsList.length - 1] === undefined
    ) {
        argumentsList.pop();
    }

    if (argumentsList.length > 0) {
        // TODO use stackAlloc/stackRestore?
        args = Module._malloc(argumentsList.length * 3 * 4);
        for (const i in argumentsList) {
            proxy_convert_js_to_mp_obj_jsside(
                argumentsList[i],
                args + i * 3 * 4,
            );
        }
    }
    const value = Module._malloc(3 * 4);
    Module.ccall(
        "proxy_c_to_js_call",
        "null",
        ["number", "number", "number", "pointer"],
        [target, argumentsList.length, args, value],
    );
    if (argumentsList.length > 0) {
        Module._free(args);
    }
    const ret = proxy_convert_mp_to_js_obj_jsside_with_free(value);
    if (ret instanceof PyProxyThenable) {
        // In Python when an async function is called it creates the
        // corresponding "generator", which must then be executed at
        // the top level by an asyncio-like scheduler.  In JavaScript
        // the semantics for async functions is that they are started
        // immediately (their non-async prefix code is executed immediately)
        // and only if they await do they return a Promise to delay the
        // execution of the remainder of the function.
        //
        // Emulate the JavaScript behaviour here by resolving the Python
        // async function.  We assume that the caller who gets this
        // return is JavaScript.
        return Promise.resolve(ret);
    }
    return ret;
}

function proxy_convert_js_to_mp_obj_jsside_helper(js_obj, out, allow_pyproxy) {
    let kind;
    if (js_obj === undefined) {
        kind = PROXY_KIND_JS_UNDEFINED;
    } else if (js_obj === null) {
        kind = PROXY_KIND_JS_NULL;
    } else if (typeof js_obj === "boolean") {
        kind = PROXY_KIND_JS_BOOLEAN;
        Module.setValue(out + 4, js_obj, "i32");
    } else if (typeof js_obj === "number") {
        if (Number.isInteger(js_obj)) {
            kind = PROXY_KIND_JS_INTEGER;
            Module.setValue(out + 4, js_obj, "i32");
        } else {
            kind = PROXY_KIND_JS_DOUBLE;
            // double must be stored to an address that's a multiple of 8
            const temp = (out + 4) & ~7;
            Module.setValue(temp, js_obj, "double");
            const double_lo = Module.getValue(temp, "i32");
            const double_hi = Module.getValue(temp + 4, "i32");
            Module.setValue(out + 4, double_lo, "i32");
            Module.setValue(out + 8, double_hi, "i32");
        }
    } else if (typeof js_obj === "string") {
        kind = PROXY_KIND_JS_STRING;
        const len = Module.lengthBytesUTF8(js_obj);
        const buf = Module._malloc(len + 1);
        Module.stringToUTF8(js_obj, buf, len + 1);
        Module.setValue(out + 4, len, "i32");
        Module.setValue(out + 8, buf, "i32");
    } else if (
        allow_pyproxy &&
        (js_obj instanceof PyProxy ||
            (typeof js_obj === "function" && "_ref" in js_obj) ||
            js_obj instanceof PyProxyThenable)
    ) {
        kind = PROXY_KIND_JS_PYPROXY;
        Module.setValue(out + 4, js_obj._ref, "i32");
    } else {
        let id;
        // See if there is an existing JsProxy reference, and use that if there is.
        const existing_ref = proxy_js_ref_map.get(js_obj);
        if (existing_ref !== undefined) {
            kind = PROXY_KIND_JS_OBJECT_EXISTING;
            id = existing_ref;
        } else {
            kind = PROXY_KIND_JS_OBJECT;
            id = proxy_js_add_obj(js_obj);
        }
        Module.setValue(out + 4, id, "i32");
    }
    Module.setValue(out + 0, kind, "i32");
}

function proxy_convert_js_to_mp_obj_jsside(js_obj, out) {
    proxy_convert_js_to_mp_obj_jsside_helper(js_obj, out, true);
}

function proxy_convert_js_to_mp_obj_jsside_force_double_proxy(js_obj, out) {
    proxy_convert_js_to_mp_obj_jsside_helper(js_obj, out, false);
}

function proxy_convert_mp_to_js_obj_jsside(value) {
    const kind = Module.getValue(value, "i32");
    let obj;
    if (kind === PROXY_KIND_MP_EXCEPTION) {
        // Exception
        const str_len = Module.getValue(value + 4, "i32");
        const str_ptr = Module.getValue(value + 8, "i32");
        const str = Module.UTF8ToString(str_ptr, str_len);
        Module._free(str_ptr);
        const str_split = str.split("\x04");
        throw new PythonError(str_split[0], str_split[1]);
    }
    if (kind === PROXY_KIND_MP_NULL) {
        // MP_OBJ_NULL
        throw new Error("NULL object");
    }
    if (kind === PROXY_KIND_MP_NONE) {
        // None
        obj = null;
    } else if (kind === PROXY_KIND_MP_BOOL) {
        // bool
        obj = Module.getValue(value + 4, "i32") ? true : false;
    } else if (kind === PROXY_KIND_MP_INT) {
        // int
        obj = Module.getValue(value + 4, "i32");
    } else if (kind === PROXY_KIND_MP_FLOAT) {
        // float
        // double must be loaded from an address that's a multiple of 8
        const temp = (value + 4) & ~7;
        const double_lo = Module.getValue(value + 4, "i32");
        const double_hi = Module.getValue(value + 8, "i32");
        Module.setValue(temp, double_lo, "i32");
        Module.setValue(temp + 4, double_hi, "i32");
        obj = Module.getValue(temp, "double");
    } else if (kind === PROXY_KIND_MP_STR) {
        // str
        const str_len = Module.getValue(value + 4, "i32");
        const str_ptr = Module.getValue(value + 8, "i32");
        obj = Module.UTF8ToString(str_ptr, str_len);
    } else if (kind === PROXY_KIND_MP_JSPROXY) {
        // js proxy
        const id = Module.getValue(value + 4, "i32");
        obj = proxy_js_ref[id];
    } else if (kind === PROXY_KIND_MP_EXISTING) {
        const id = Module.getValue(value + 4, "i32");
        obj = globalThis.proxy_js_existing[id];
        globalThis.proxy_js_existing[id] = undefined;
    } else {
        // obj
        const id = Module.getValue(value + 4, "i32");
        if (kind === PROXY_KIND_MP_CALLABLE) {
            obj = (...args) => {
                return proxy_call_python(id, args);
            };
            obj._ref = id;
        } else if (kind === PROXY_KIND_MP_GENERATOR) {
            obj = new PyProxyThenable(id);
        } else {
            // PROXY_KIND_MP_OBJECT
            const target = new PyProxy(id);
            obj = new Proxy(target, py_proxy_handler);
        }
        globalThis.pyProxyFinalizationRegistry.register(obj, id);
        globalThis.proxy_js_map.set(id, new WeakRef(obj));
    }
    return obj;
}

function proxy_convert_mp_to_js_obj_jsside_with_free(value) {
    const ret = proxy_convert_mp_to_js_obj_jsside(value);
    Module._free(value);
    return ret;
}

function python_index_semantics(target, index_in) {
    let index = index_in;
    if (typeof index === "number") {
        if (index < 0) {
            index += target.length;
        }
        if (index < 0 || index >= target.length) {
            throw new PythonError("IndexError", "index out of range");
        }
    }
    return index;
}
