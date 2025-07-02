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

class PyProxy {
    constructor(ref) {
        this._ref = ref;
    }

    // Convert js_obj -- which is possibly a PyProxy -- to a JavaScript object.
    static toJs(js_obj) {
        if (!(js_obj instanceof PyProxy)) {
            return js_obj;
        }

        const type = Module.ccall(
            "proxy_c_to_js_get_type",
            "number",
            ["number"],
            [js_obj._ref],
        );

        if (type === 1 || type === 2) {
            // List or tuple.
            const array_ref = Module._malloc(2 * 4);
            const item = Module._malloc(3 * 4);
            Module.ccall(
                "proxy_c_to_js_get_array",
                "null",
                ["number", "pointer"],
                [js_obj._ref, array_ref],
            );
            const len = Module.getValue(array_ref, "i32");
            const items_ptr = Module.getValue(array_ref + 4, "i32");
            const js_array = [];
            for (let i = 0; i < len; ++i) {
                Module.ccall(
                    "proxy_convert_mp_to_js_obj_cside",
                    "null",
                    ["pointer", "pointer"],
                    [Module.getValue(items_ptr + i * 4, "i32"), item],
                );
                const js_item = proxy_convert_mp_to_js_obj_jsside(item);
                js_array.push(PyProxy.toJs(js_item));
            }
            Module._free(array_ref);
            Module._free(item);
            return js_array;
        }

        if (type === 3) {
            // Dict.
            const map_ref = Module._malloc(2 * 4);
            const item = Module._malloc(3 * 4);
            Module.ccall(
                "proxy_c_to_js_get_dict",
                "null",
                ["number", "pointer"],
                [js_obj._ref, map_ref],
            );
            const alloc = Module.getValue(map_ref, "i32");
            const table_ptr = Module.getValue(map_ref + 4, "i32");
            const js_dict = {};
            for (let i = 0; i < alloc; ++i) {
                const mp_key = Module.getValue(table_ptr + i * 8, "i32");
                if (mp_key > 8) {
                    // Convert key to JS object.
                    Module.ccall(
                        "proxy_convert_mp_to_js_obj_cside",
                        "null",
                        ["pointer", "pointer"],
                        [mp_key, item],
                    );
                    const js_key = proxy_convert_mp_to_js_obj_jsside(item);

                    // Convert value to JS object.
                    const mp_value = Module.getValue(
                        table_ptr + i * 8 + 4,
                        "i32",
                    );
                    Module.ccall(
                        "proxy_convert_mp_to_js_obj_cside",
                        "null",
                        ["pointer", "pointer"],
                        [mp_value, item],
                    );
                    const js_value = proxy_convert_mp_to_js_obj_jsside(item);

                    // Populate JS dict.
                    js_dict[js_key] = PyProxy.toJs(js_value);
                }
            }
            Module._free(map_ref);
            Module._free(item);
            return js_dict;
        }

        // Cannot convert to JS, leave as a PyProxy.
        return js_obj;
    }
}

// This handler's goal is to allow minimal introspection
// of Python references from the JS world/utilities.
const py_proxy_handler = {
    isExtensible() {
        return true;
    },
    ownKeys(target) {
        const value = Module._malloc(3 * 4);
        Module.ccall(
            "proxy_c_to_js_dir",
            "null",
            ["number", "pointer"],
            [target._ref, value],
        );
        const dir = proxy_convert_mp_to_js_obj_jsside_with_free(value);
        return PyProxy.toJs(dir).filter((attr) => !attr.startsWith("__"));
    },
    getOwnPropertyDescriptor(target, prop) {
        return {
            value: target[prop],
            enumerable: true,
            writable: true,
            configurable: true,
        };
    },
    has(target, prop) {
        // avoid throwing on `Symbol() in proxy` checks
        if (typeof prop !== "string") {
            // returns true only on iterator because other
            // symbols are not considered in the `get` trap
            return prop === Symbol.iterator;
        }
        return Module.ccall(
            "proxy_c_to_js_has_attr",
            "number",
            ["number", "string"],
            [target._ref, prop],
        );
    },
    get(target, prop) {
        if (prop === "_ref") {
            return target._ref;
        }
        if (prop === "then") {
            return null;
        }

        if (prop === Symbol.iterator) {
            // Get the Python object iterator, and return a JavaScript generator.
            const iter_ref = Module.ccall(
                "proxy_c_to_js_get_iter",
                "number",
                ["number"],
                [target._ref],
            );
            return function* () {
                const value = Module._malloc(3 * 4);
                while (true) {
                    const valid = Module.ccall(
                        "proxy_c_to_js_iternext",
                        "number",
                        ["number", "pointer"],
                        [iter_ref, value],
                    );
                    if (!valid) {
                        break;
                    }
                    yield proxy_convert_mp_to_js_obj_jsside(value);
                }
                Module._free(value);
            };
        }

        const value = Module._malloc(3 * 4);
        Module.ccall(
            "proxy_c_to_js_lookup_attr",
            "null",
            ["number", "string", "pointer"],
            [target._ref, prop, value],
        );
        return proxy_convert_mp_to_js_obj_jsside_with_free(value);
    },
    set(target, prop, value) {
        const value_conv = Module._malloc(3 * 4);
        proxy_convert_js_to_mp_obj_jsside(value, value_conv);
        const ret = Module.ccall(
            "proxy_c_to_js_store_attr",
            "number",
            ["number", "string", "number"],
            [target._ref, prop, value_conv],
        );
        Module._free(value_conv);
        return ret;
    },
    deleteProperty(target, prop) {
        return Module.ccall(
            "proxy_c_to_js_delete_attr",
            "number",
            ["number", "string"],
            [target._ref, prop],
        );
    },
};

// PyProxy of a Python generator, that implements the thenable interface.
class PyProxyThenable {
    constructor(ref) {
        this._ref = ref;
    }

    then(resolve, reject) {
        const values = Module._malloc(3 * 3 * 4);
        proxy_convert_js_to_mp_obj_jsside(resolve, values + 3 * 4);
        proxy_convert_js_to_mp_obj_jsside(reject, values + 2 * 3 * 4);
        Module.ccall(
            "proxy_c_to_js_resume",
            "null",
            ["number", "pointer"],
            [this._ref, values],
        );
        return proxy_convert_mp_to_js_obj_jsside_with_free(values);
    }
}
