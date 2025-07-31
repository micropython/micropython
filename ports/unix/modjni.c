/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
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

#include "py/runtime.h"
#include "py/binary.h"

#if MICROPY_PY_JNI

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include <ctype.h>

#include <jni.h>

#define JJ(call, ...) (*env)->call(env, __VA_ARGS__)
#define JJ1(call) (*env)->call(env)
#define MATCH(s, static) (!strncmp(s, static, sizeof(static) - 1))

static JavaVM *jvm;
static JNIEnv *env;
static jclass Class_class;
static jclass String_class;
static jmethodID Class_getName_mid;
static jmethodID Class_getField_mid;
static jmethodID Class_getMethods_mid;
static jmethodID Class_getConstructors_mid;
static jmethodID Method_getName_mid;
static jmethodID Object_toString_mid;

static jclass List_class;
static jmethodID List_get_mid;
static jmethodID List_set_mid;
static jmethodID List_size_mid;

static jclass IndexException_class;

static const mp_obj_type_t jobject_type;
static const mp_obj_type_t jmethod_type;

static mp_obj_t new_jobject(jobject jo);
static mp_obj_t new_jclass(jclass jc);
static mp_obj_t call_method(jobject obj, const char *name, jarray methods, bool is_constr, size_t n_args, const mp_obj_t *args);
static bool py2jvalue(const char **jtypesig, mp_obj_t arg, jvalue *out);

typedef struct _mp_obj_jclass_t {
    mp_obj_base_t base;
    jclass cls;
} mp_obj_jclass_t;

typedef struct _mp_obj_jobject_t {
    mp_obj_base_t base;
    jobject obj;
} mp_obj_jobject_t;

typedef struct _mp_obj_jmethod_t {
    mp_obj_base_t base;
    jobject obj;
    jmethodID meth;
    qstr name;
    bool is_static;
} mp_obj_jmethod_t;

// Utility functions

static bool is_object_type(const char *jtypesig) {
    while (*jtypesig != ' ' && *jtypesig) {
        if (*jtypesig == '.') {
            return true;
        }
        jtypesig++;
    }
    return false;
}

static void check_exception(void) {
    jobject exc = JJ1(ExceptionOccurred);
    if (exc) {
        // JJ1(ExceptionDescribe);
        mp_obj_t py_e = new_jobject(exc);
        JJ1(ExceptionClear);
        if (JJ(IsInstanceOf, exc, IndexException_class)) {
            mp_raise_type_arg(&mp_type_IndexError, py_e);
        }
        mp_raise_type_arg(&mp_type_Exception, py_e);
    }
}

static void print_jobject(const mp_print_t *print, jobject obj) {
    jobject str_o = JJ(CallObjectMethod, obj, Object_toString_mid);
    const char *str = JJ(GetStringUTFChars, str_o, NULL);
    mp_printf(print, str);
    JJ(ReleaseStringUTFChars, str_o, str);
}

// jclass

static void jclass_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_jclass_t *self = MP_OBJ_TO_PTR(self_in);
    if (kind == PRINT_REPR) {
        mp_printf(print, "<jclass @%p \"", self->cls);
    }
    print_jobject(print, self->cls);
    if (kind == PRINT_REPR) {
        mp_printf(print, "\">");
    }
}

static void jclass_attr(mp_obj_t self_in, qstr attr_in, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        mp_obj_jclass_t *self = MP_OBJ_TO_PTR(self_in);
        const char *attr = qstr_str(attr_in);

        jstring field_name = JJ(NewStringUTF, attr);
        jobject field = JJ(CallObjectMethod, self->cls, Class_getField_mid, field_name);
        if (!JJ1(ExceptionCheck)) {
            jfieldID field_id = JJ(FromReflectedField, field);
            jobject obj = JJ(GetStaticObjectField, self->cls, field_id);
            dest[0] = new_jobject(obj);
            return;
        }
        // JJ1(ExceptionDescribe);
        JJ1(ExceptionClear);

        mp_obj_jmethod_t *o = mp_obj_malloc(mp_obj_jmethod_t, &jmethod_type);
        o->name = attr_in;
        o->meth = NULL;
        o->obj = self->cls;
        o->is_static = true;
        dest[0] = MP_OBJ_FROM_PTR(o);
    }
}

static mp_obj_t jclass_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    if (n_kw != 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("kwargs not supported"));
    }
    mp_obj_jclass_t *self = MP_OBJ_TO_PTR(self_in);

    jarray methods = JJ(CallObjectMethod, self->cls, Class_getConstructors_mid);

    return call_method(self->cls, NULL, methods, true, n_args, args);
}

static const mp_rom_map_elem_t jclass_locals_dict_table[] = {
//    { MP_ROM_QSTR(MP_QSTR_get), MP_ROM_PTR(&ffivar_get_obj) },
//    { MP_ROM_QSTR(MP_QSTR_set), MP_ROM_PTR(&ffivar_set_obj) },
};

static MP_DEFINE_CONST_DICT(jclass_locals_dict, jclass_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    jclass_type,
    MP_QSTR_jclass,
    MP_TYPE_FLAG_NONE,
    print, jclass_print,
    attr, jclass_attr,
    call, jclass_call,
    locals_dict, &jclass_locals_dict
    );

static mp_obj_t new_jclass(jclass jc) {
    mp_obj_jclass_t *o = mp_obj_malloc(mp_obj_jclass_t, &jclass_type);
    o->cls = jc;
    return MP_OBJ_FROM_PTR(o);
}

// jobject

static void jobject_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_jobject_t *self = MP_OBJ_TO_PTR(self_in);
    if (kind == PRINT_REPR) {
        mp_printf(print, "<jobject @%p \"", self->obj);
    }
    print_jobject(print, self->obj);
    if (kind == PRINT_REPR) {
        mp_printf(print, "\">");
    }
}

static void jobject_attr(mp_obj_t self_in, qstr attr_in, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        mp_obj_jobject_t *self = MP_OBJ_TO_PTR(self_in);

        const char *attr = qstr_str(attr_in);
        jclass obj_class = JJ(GetObjectClass, self->obj);
        jstring field_name = JJ(NewStringUTF, attr);
        jobject field = JJ(CallObjectMethod, obj_class, Class_getField_mid, field_name);
        JJ(DeleteLocalRef, field_name);
        JJ(DeleteLocalRef, obj_class);
        if (!JJ1(ExceptionCheck)) {
            jfieldID field_id = JJ(FromReflectedField, field);
            JJ(DeleteLocalRef, field);
            jobject obj = JJ(GetObjectField, self->obj, field_id);
            dest[0] = new_jobject(obj);
            return;
        }
        // JJ1(ExceptionDescribe);
        JJ1(ExceptionClear);

        mp_obj_jmethod_t *o = mp_obj_malloc(mp_obj_jmethod_t, &jmethod_type);
        o->name = attr_in;
        o->meth = NULL;
        o->obj = self->obj;
        o->is_static = false;
        dest[0] = MP_OBJ_FROM_PTR(o);
    }
}

static void get_jclass_name(jobject obj, char *buf) {
    jclass obj_class = JJ(GetObjectClass, obj);
    jstring name = JJ(CallObjectMethod, obj_class, Class_getName_mid);
    jint len = JJ(GetStringLength, name);
    JJ(GetStringUTFRegion, name, 0, len, buf);
    check_exception();
}

static mp_obj_t jobject_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_jobject_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t idx = mp_obj_get_int(index);
    char class_name[64];
    get_jclass_name(self->obj, class_name);
    // printf("class: %s\n", class_name);

    if (class_name[0] == '[') {
        if (class_name[1] == 'L' || class_name[1] == '[') {
            if (value == MP_OBJ_NULL) {
                // delete
                assert(0);
            } else if (value == MP_OBJ_SENTINEL) {
                // load
                jobject el = JJ(GetObjectArrayElement, self->obj, idx);
                return new_jobject(el);
            } else {
                // store
                jvalue jval;
                const char *t = class_name + 1;
                py2jvalue(&t, value, &jval);
                JJ(SetObjectArrayElement, self->obj, idx, jval.l);
                return mp_const_none;
            }
        }
        mp_raise_NotImplementedError(NULL);
    }

    if (!JJ(IsInstanceOf, self->obj, List_class)) {
        return MP_OBJ_NULL;
    }


    if (value == MP_OBJ_NULL) {
        // delete
        assert(0);
    } else if (value == MP_OBJ_SENTINEL) {
        // load
        jobject el = JJ(CallObjectMethod, self->obj, List_get_mid, idx);
        check_exception();
        return new_jobject(el);
    } else {
        // store
        assert(0);
    }


    return MP_OBJ_NULL;
}

static mp_obj_t jobject_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    mp_obj_jobject_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_BOOL:
        case MP_UNARY_OP_LEN: {
            jint len = JJ(CallIntMethod, self->obj, List_size_mid);
            if (op == MP_UNARY_OP_BOOL) {
                return mp_obj_new_bool(len != 0);
            }
            return MP_OBJ_NEW_SMALL_INT(len);
        }
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

// TODO: subscr_load_adaptor & subscr_getiter convenience functions
// should be moved to common location for reuse.
static mp_obj_t subscr_load_adaptor(mp_obj_t self_in, mp_obj_t index_in) {
    return mp_obj_subscr(self_in, index_in, MP_OBJ_SENTINEL);
}
MP_DEFINE_CONST_FUN_OBJ_2(subscr_load_adaptor_obj, subscr_load_adaptor);

// .getiter special method which returns iterator which works in terms
// of object subscription.
static mp_obj_t subscr_getiter(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf) {
    mp_obj_t dest[2] = {MP_OBJ_FROM_PTR(&subscr_load_adaptor_obj), self_in};
    return mp_obj_new_getitem_iter(dest, iter_buf);
}

static MP_DEFINE_CONST_OBJ_TYPE(
    jobject_type,
    MP_QSTR_jobject,
    MP_TYPE_FLAG_ITER_IS_GETITER,
    print, jobject_print,
    unary_op, jobject_unary_op,
    attr, jobject_attr,
    subscr, jobject_subscr,
    iter, subscr_getiter
    );

static mp_obj_t new_jobject(jobject jo) {
    if (jo == NULL) {
        return mp_const_none;
    } else if (JJ(IsInstanceOf, jo, String_class)) {
        const char *s = JJ(GetStringUTFChars, jo, NULL);
        mp_obj_t ret = mp_obj_new_str_from_cstr(s);
        JJ(ReleaseStringUTFChars, jo, s);
        return ret;
    } else if (JJ(IsInstanceOf, jo, Class_class)) {
        return new_jclass(jo);
    } else {
        mp_obj_jobject_t *o = mp_obj_malloc(mp_obj_jobject_t, &jobject_type);
        o->obj = jo;
        return MP_OBJ_FROM_PTR(o);
    }

}


// jmethod

static void jmethod_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_jmethod_t *self = MP_OBJ_TO_PTR(self_in);
    // Variable value printed as cast to int
    mp_printf(print, "<jmethod '%s'>", qstr_str(self->name));
}

#define IMATCH(s, static) ((!strncmp(s, static, sizeof(static) - 1)) && (s += sizeof(static) - 1))

#define CHECK_TYPE(java_type_name) \
    if (strncmp(arg_type, java_type_name, sizeof(java_type_name) - 1) != 0) { \
        return false; \
    } \
    arg_type += sizeof(java_type_name) - 1;

static const char *strprev(const char *s, char c) {
    while (*s != c) {
        s--;
    }
    return s;
}

static bool py2jvalue(const char **jtypesig, mp_obj_t arg, jvalue *out) {
    const char *arg_type = *jtypesig;
    const mp_obj_type_t *type = mp_obj_get_type(arg);

    if (type == &mp_type_str) {
        if (IMATCH(arg_type, "java.lang.String") || IMATCH(arg_type, "java.lang.Object")) {
            out->l = JJ(NewStringUTF, mp_obj_str_get_str(arg));
        } else {
            return false;
        }
    } else if (type == &mp_type_int) {
        if (IMATCH(arg_type, "int") || IMATCH(arg_type, "long")) {
            // TODO: Java long is 64-bit actually
            out->j = mp_obj_get_int(arg);
        } else {
            return false;
        }
    } else if (type == &jobject_type) {
        bool is_object = false;
        const char *expected_type = arg_type;
        while (1) {
            if (isalpha(*arg_type)) {
            } else if (*arg_type == '.') {
                is_object = true;
            } else {
                break;
            }
            arg_type++;
        }
        if (!is_object) {
            return false;
        }
        mp_obj_jobject_t *jo = MP_OBJ_TO_PTR(arg);
        if (!MATCH(expected_type, "java.lang.Object")) {
            char class_name[64];
            get_jclass_name(jo->obj, class_name);
            // printf("Arg class: %s\n", class_name);
            if (strcmp(class_name, expected_type) != 0) {
                return false;
            }
        }
        out->l = jo->obj;
    } else if (type == &mp_type_bool) {
        if (IMATCH(arg_type, "boolean")) {
            out->z = arg == mp_const_true;
        } else {
            return false;
        }
    } else if (arg == mp_const_none) {
        // printf("TODO: Check java arg type!!\n");
        while (isalpha(*arg_type) || *arg_type == '.') {
            arg_type++;
        }
        out->l = NULL;
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("arg type not supported"));
    }

    *jtypesig = arg_type;
    return true;
}

#if 0
// jvalue is known to be union of jobject and friends. And yet from C's
// perspective, it's aggregate object which may require passing via stack
// instead of registers. Work that around by passing jobject and typecasting
// it.
static mp_obj_t jvalue2py(const char *jtypesig, jobject arg) {
    if (arg == NULL || MATCH(jtypesig, "void")) {
        return mp_const_none;
    } else if (MATCH(jtypesig, "boolean")) {
        return mp_obj_new_bool((bool)arg);
    } else if (MATCH(jtypesig, "int")) {
        return mp_obj_new_int((mp_int_t)arg);
    } else if (is_object_type(jtypesig)) {
        // Non-primitive, object type
        return new_jobject(arg);
    }

    printf("Unknown return type: %s\n", jtypesig);

    return MP_OBJ_NULL;
}
#endif

static mp_obj_t call_method(jobject obj, const char *name, jarray methods, bool is_constr, size_t n_args, const mp_obj_t *args) {
    jvalue jargs[n_args];
//    printf("methods=%p\n", methods);
    jsize num_methods = JJ(GetArrayLength, methods);
    for (int i = 0; i < num_methods; i++) {
        jobject meth = JJ(GetObjectArrayElement, methods, i);
        jobject name_o = JJ(CallObjectMethod, meth, Object_toString_mid);
        const char *decl = JJ(GetStringUTFChars, name_o, NULL);
        const char *arg_types = strchr(decl, '(') + 1;
        // const char *arg_types_end = strchr(arg_types, ')');
//        printf("method[%d]=%p %s\n", i, meth, decl);

        const char *meth_name = NULL;
        const char *ret_type = NULL;
        if (!is_constr) {
            meth_name = strprev(arg_types, '.') + 1;
            ret_type = strprev(meth_name, ' ') - 1;
            ret_type = strprev(ret_type, ' ') + 1;

            int name_len = strlen(name);
            if (strncmp(name, meth_name, name_len /*arg_types - meth_name - 1*/) || meth_name[name_len] != '(' /*(*/) {
                goto next_method;
            }
        }
//        printf("method[%d]=%p %s\n", i, meth, decl);
//        printf("!!!%s\n", arg_types);
//        printf("name=%p meth_name=%s\n", name, meth_name);

        bool found = true;
        for (size_t j = 0; j < n_args && *arg_types != ')'; j++) {
            if (!py2jvalue(&arg_types, args[j], &jargs[j])) {
                goto next_method;
            }

            if (*arg_types == ',') {
                arg_types++;
            }
        }

        if (*arg_types != ')') {
            goto next_method;
        }

        if (found) {
//            printf("found!\n");
            jmethodID method_id = JJ(FromReflectedMethod, meth);
            if (is_constr) {
                JJ(ReleaseStringUTFChars, name_o, decl);
                jobject res = JJ(NewObjectA, obj, method_id, jargs);
                return new_jobject(res);
            } else {
                mp_obj_t ret;
                if (MATCH(ret_type, "void")) {
                    JJ(CallVoidMethodA, obj, method_id, jargs);
                    check_exception();
                    ret = mp_const_none;
                } else if (MATCH(ret_type, "int")) {
                    jint res = JJ(CallIntMethodA, obj, method_id, jargs);
                    check_exception();
                    ret = mp_obj_new_int(res);
                } else if (MATCH(ret_type, "boolean")) {
                    jboolean res = JJ(CallBooleanMethodA, obj, method_id, jargs);
                    check_exception();
                    ret = mp_obj_new_bool(res);
                } else if (is_object_type(ret_type)) {
                    jobject res = JJ(CallObjectMethodA, obj, method_id, jargs);
                    check_exception();
                    ret = new_jobject(res);
                } else {
                    JJ(ReleaseStringUTFChars, name_o, decl);
                    mp_raise_TypeError(MP_ERROR_TEXT("can't handle return type"));
                }

                JJ(ReleaseStringUTFChars, name_o, decl);
                JJ(DeleteLocalRef, name_o);
                JJ(DeleteLocalRef, meth);
                return ret;
            }
        }

    next_method:
        JJ(ReleaseStringUTFChars, name_o, decl);
        JJ(DeleteLocalRef, name_o);
        JJ(DeleteLocalRef, meth);
    }

    mp_raise_TypeError(MP_ERROR_TEXT("method not found"));
}


static mp_obj_t jmethod_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    if (n_kw != 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("kwargs not supported"));
    }
    mp_obj_jmethod_t *self = MP_OBJ_TO_PTR(self_in);

    const char *name = qstr_str(self->name);
//    jstring meth_name = JJ(NewStringUTF, name);

    jclass obj_class = self->obj;
    if (!self->is_static) {
        obj_class = JJ(GetObjectClass, self->obj);
    }
    jarray methods = JJ(CallObjectMethod, obj_class, Class_getMethods_mid);

    return call_method(self->obj, name, methods, false, n_args, args);
}

static MP_DEFINE_CONST_OBJ_TYPE(
    jmethod_type,
    MP_QSTR_jmethod,
    MP_TYPE_FLAG_NONE,
    print, jmethod_print,
    call, jmethod_call
    );

#ifdef __ANDROID__
#define LIBJVM_SO "libdvm.so"
#else
#define LIBJVM_SO "libjvm.so"
#endif

static void create_jvm(void) {
    JavaVMInitArgs args;
    JavaVMOption options;
    options.optionString = "-Djava.class.path=.";
    args.version = JNI_VERSION_1_6;
    args.nOptions = 1;
    args.options = &options;
    args.ignoreUnrecognized = 0;

    if (env) {
        return;
    }

    void *libjvm = dlopen(LIBJVM_SO, RTLD_NOW | RTLD_GLOBAL);
    if (!libjvm) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("unable to load libjvm.so, use LD_LIBRARY_PATH"));
    }
    int (*_JNI_CreateJavaVM)(void *, void **, void *) = dlsym(libjvm, "JNI_CreateJavaVM");

    int st = _JNI_CreateJavaVM(&jvm, (void **)&env, &args);
    if (st < 0 || !env) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("unable to create JVM"));
    }

    Class_class = JJ(FindClass, "java/lang/Class");
    jclass method_class = JJ(FindClass, "java/lang/reflect/Method");
    String_class = JJ(FindClass, "java/lang/String");

    jclass Object_class = JJ(FindClass, "java/lang/Object");
    Object_toString_mid = JJ(GetMethodID, Object_class, "toString",
        MP_COMPRESSED_ROM_TEXT("()Ljava/lang/String;"));

    Class_getName_mid = (*env)->GetMethodID(env, Class_class, "getName",
        MP_COMPRESSED_ROM_TEXT("()Ljava/lang/String;"));
    Class_getField_mid = (*env)->GetMethodID(env, Class_class, "getField",
        MP_COMPRESSED_ROM_TEXT("(Ljava/lang/String;)Ljava/lang/reflect/Field;"));
    Class_getMethods_mid = (*env)->GetMethodID(env, Class_class, "getMethods",
        MP_COMPRESSED_ROM_TEXT("()[Ljava/lang/reflect/Method;"));
    Class_getConstructors_mid = (*env)->GetMethodID(env, Class_class, "getConstructors",
        MP_COMPRESSED_ROM_TEXT("()[Ljava/lang/reflect/Constructor;"));
    Method_getName_mid = (*env)->GetMethodID(env, method_class, "getName",
        MP_COMPRESSED_ROM_TEXT("()Ljava/lang/String;"));

    List_class = JJ(FindClass, "java/util/List");
    List_get_mid = JJ(GetMethodID, List_class, "get",
        MP_COMPRESSED_ROM_TEXT("(I)Ljava/lang/Object;"));
    List_set_mid = JJ(GetMethodID, List_class, "set",
        MP_COMPRESSED_ROM_TEXT("(ILjava/lang/Object;)Ljava/lang/Object;"));
    List_size_mid = JJ(GetMethodID, List_class, "size",
        MP_COMPRESSED_ROM_TEXT("()I"));
    IndexException_class = JJ(FindClass, "java/lang/IndexOutOfBoundsException");
}

static mp_obj_t mod_jni_cls(mp_obj_t cls_name_in) {
    const char *cls_name = mp_obj_str_get_str(cls_name_in);
    if (!env) {
        create_jvm();
    }
    jclass cls = JJ(FindClass, cls_name);

    mp_obj_jclass_t *o = mp_obj_malloc(mp_obj_jclass_t, &jclass_type);
    o->cls = cls;
    return MP_OBJ_FROM_PTR(o);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_jni_cls_obj, mod_jni_cls);

static mp_obj_t mod_jni_array(mp_obj_t type_in, mp_obj_t size_in) {
    if (!env) {
        create_jvm();
    }
    mp_int_t size = mp_obj_get_int(size_in);
    jobject res = NULL;

    if (mp_obj_is_type(type_in, &jclass_type)) {

        mp_obj_jclass_t *jcls = MP_OBJ_TO_PTR(type_in);
        res = JJ(NewObjectArray, size, jcls->cls, NULL);

    } else if (mp_obj_is_str(type_in)) {
        const char *type = mp_obj_str_get_str(type_in);
        switch (*type) {
            case 'Z':
                res = JJ(NewBooleanArray, size);
                break;
            case 'B':
                res = JJ(NewByteArray, size);
                break;
            case 'C':
                res = JJ(NewCharArray, size);
                break;
            case 'S':
                res = JJ(NewShortArray, size);
                break;
            case 'I':
                res = JJ(NewIntArray, size);
                break;
            case 'J':
                res = JJ(NewLongArray, size);
                break;
            case 'F':
                res = JJ(NewFloatArray, size);
                break;
            case 'D':
                res = JJ(NewDoubleArray, size);
                break;
        }

    }

    return new_jobject(res);
}
MP_DEFINE_CONST_FUN_OBJ_2(mod_jni_array_obj, mod_jni_array);


static mp_obj_t mod_jni_env(void) {
    return mp_obj_new_int((mp_int_t)(uintptr_t)env);
}
MP_DEFINE_CONST_FUN_OBJ_0(mod_jni_env_obj, mod_jni_env);

static const mp_rom_map_elem_t mp_module_jni_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_jni) },
    { MP_ROM_QSTR(MP_QSTR_cls), MP_ROM_PTR(&mod_jni_cls_obj) },
    { MP_ROM_QSTR(MP_QSTR_array), MP_ROM_PTR(&mod_jni_array_obj) },
    { MP_ROM_QSTR(MP_QSTR_env), MP_ROM_PTR(&mod_jni_env_obj) },
};

static MP_DEFINE_CONST_DICT(mp_module_jni_globals, mp_module_jni_globals_table);

const mp_obj_module_t mp_module_jni = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_jni_globals,
};

MP_REGISTER_MODULE(MP_QSTR_jni, mp_module_jni);

#endif // MICROPY_PY_JNI
