/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/binary.h"

#include <jni.h>

#define JJ(call, ...) (*env)->call(env, __VA_ARGS__)
#define JJ1(call) (*env)->call(env)

static JavaVM *jvm;
static JNIEnv *env;
static jclass Class_class;
static jclass String_class;
static jmethodID Class_getField_mid;
static jmethodID Class_getMethods_mid;
static jmethodID Class_getConstructors_mid;
static jmethodID Method_getName_mid;
static jmethodID Method_toString_mid;

static jclass List_class;
static jmethodID List_get_mid;
static jmethodID List_set_mid;

STATIC const mp_obj_type_t jobject_type;
STATIC const mp_obj_type_t jmethod_type;

STATIC mp_obj_t new_jobject(jobject jo);
STATIC mp_obj_t new_jclass(jclass jc);
STATIC mp_obj_t call_method(jobject obj, const char *name, jarray methods, bool is_constr, mp_uint_t n_args, const mp_obj_t *args);

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

// jclass

STATIC void jclass_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_jclass_t *self = self_in;
    // Variable value printed as cast to int
    mp_printf(print, "<jclass @%p>", self->cls);
}

STATIC void jclass_attr(mp_obj_t self_in, qstr attr_in, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        mp_obj_jclass_t *self = self_in;
        const char *attr = qstr_str(attr_in);

        jstring field_name = JJ(NewStringUTF, attr);
        jobject field = JJ(CallObjectMethod, self->cls, Class_getField_mid, field_name);
        if (!JJ1(ExceptionCheck)) {
            jfieldID field_id = JJ(FromReflectedField, field);
            jobject obj = JJ(GetStaticObjectField, self->cls, field_id);
            dest[0] = new_jobject(obj);
            return;
        }
        //JJ1(ExceptionDescribe);
        JJ1(ExceptionClear);

        mp_obj_jmethod_t *o = m_new_obj(mp_obj_jmethod_t);
        o->base.type = &jmethod_type;
        o->name = attr_in;
        o->meth = NULL;
        o->obj = self->cls;
        o->is_static = true;
        dest[0] = o;
    }
}

STATIC mp_obj_t jclass_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    if (n_kw != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "kwargs not supported"));
    }
    mp_obj_jclass_t *self = self_in;

    jarray methods = JJ(CallObjectMethod, self->cls, Class_getConstructors_mid);

    return call_method(self->cls, NULL, methods, true, n_args, args);
}

STATIC const mp_map_elem_t jclass_locals_dict_table[] = {
//    { MP_OBJ_NEW_QSTR(MP_QSTR_get), (mp_obj_t)&ffivar_get_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_set), (mp_obj_t)&ffivar_set_obj },
};

STATIC MP_DEFINE_CONST_DICT(jclass_locals_dict, jclass_locals_dict_table);

STATIC const mp_obj_type_t jclass_type = {
    { &mp_type_type },
    .name = MP_QSTR_jclass,
    .print = jclass_print,
    .attr = jclass_attr,
    .call = jclass_call,
    .locals_dict = (mp_obj_t)&jclass_locals_dict,
};

STATIC mp_obj_t new_jclass(jclass jc) {
    mp_obj_jclass_t *o = m_new_obj(mp_obj_jclass_t);
    o->base.type = &jclass_type;
    o->cls = jc;
    return o;
}

// jobject

STATIC void jobject_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_jobject_t *self = self_in;
    // Variable value printed as cast to int
    mp_printf(print, "<jobject @%p>", self->obj);
}

STATIC void jobject_attr(mp_obj_t self_in, qstr attr_in, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        mp_obj_jobject_t *self = self_in;

        mp_obj_jmethod_t *o = m_new_obj(mp_obj_jmethod_t);
        o->base.type = &jmethod_type;
        o->name = attr_in;
        o->meth = NULL;
        o->obj = self->obj;
        o->is_static = false;
        dest[0] = o;
    }
}

STATIC mp_obj_t jobject_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_jobject_t *self = self_in;
    if (!JJ(IsInstanceOf, self->obj, List_class)) {
        return MP_OBJ_NULL;
    }

    mp_uint_t idx = mp_obj_get_int(index);

    if (value == MP_OBJ_NULL) {
        // delete
        assert(0);
    } else if (value == MP_OBJ_SENTINEL) {
        // load
        jobject el = JJ(CallObjectMethod, self->obj, List_get_mid, idx);
        return new_jobject(el);
    } else {
        // store
        assert(0);
    }


return MP_OBJ_NULL;
}

STATIC const mp_obj_type_t jobject_type = {
    { &mp_type_type },
    .name = MP_QSTR_jobject,
    .print = jobject_print,
    .attr = jobject_attr,
    .subscr = jobject_subscr,
//    .locals_dict = (mp_obj_t)&jobject_locals_dict,
};

STATIC mp_obj_t new_jobject(jobject jo) {
    mp_obj_jobject_t *o = m_new_obj(mp_obj_jobject_t);
    o->base.type = &jobject_type;
    o->obj = jo;
    return o;
}


// jmethod

STATIC void jmethod_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_jmethod_t *self = self_in;
    // Variable value printed as cast to int
    mp_printf(print, "<jmethod '%s'>", qstr_str(self->name));
}

#define IMATCH(s, static) ((!strncmp(s, static, sizeof(static) - 1)) && (s += sizeof(static) - 1))

#define CHECK_TYPE(java_type_name) \
                if (strncmp(arg_type, java_type_name, sizeof(java_type_name) - 1) != 0) { \
                    return false; \
                } \
                arg_type += sizeof(java_type_name) - 1;

STATIC const char *strprev(const char *s, char c) {
    while (*s != c) {
        s--;
    }
    return s;
}

STATIC bool py2jvalue(const char **jtypesig, mp_obj_t arg, jvalue *out) {
    const char *arg_type = *jtypesig;
    mp_obj_type_t *type = mp_obj_get_type(arg);

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
        printf("TODO: Check java arg type!!\n");
        mp_obj_jobject_t *jo = arg;
        out->l = jo->obj;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "arg type not supported"));
    }

    *jtypesig = arg_type;
    return true;
}

// jvalue is known to be union of jobject and friends. And yet from C's
// perspective, it's aggregate object which may require passing via stack
// instead of registers. Work that around by passing jobject and typecasting
// it.
#define MATCH(s, static) (!strncmp(s, static, sizeof(static) - 1))
STATIC mp_obj_t jvalue2py(const char *jtypesig, jobject arg) {
    const char *org_jtype = jtypesig;
    mp_obj_t ret;
    if (arg == NULL || MATCH(jtypesig, "void")) {
        return mp_const_none;
    } else if (MATCH(jtypesig, "boolean")) {
        return mp_obj_new_bool((bool)arg);
    } else if (MATCH(jtypesig, "int")) {
        return mp_obj_new_int((mp_int_t)arg);
    } else if (MATCH(jtypesig, "java.lang.String")) {
ret_string:;
        const char *s = JJ(GetStringUTFChars, arg, NULL);
        ret = mp_obj_new_str(s, strlen(s), false);
        JJ(ReleaseStringUTFChars, arg, s);
        return ret;
    } else {
        while (*jtypesig != ' ' && *jtypesig) {
            if (*jtypesig == '.') {
                // Non-primitive, object type
                if (JJ(IsInstanceOf, arg, String_class)) {
                    goto ret_string;
                } else if (JJ(IsInstanceOf, arg, Class_class)) {
                    return new_jclass(arg);
                } else {
                    return new_jobject(arg);
                }
            }
            jtypesig++;
        }
    }

    printf("Unknown return type: %s\n", org_jtype);

    return MP_OBJ_NULL;
}

STATIC mp_obj_t call_method(jobject obj, const char *name, jarray methods, bool is_constr, mp_uint_t n_args, const mp_obj_t *args) {
    jvalue jargs[n_args];
//    printf("methods=%p\n", methods);
    jsize num_methods = JJ(GetArrayLength, methods);
    for (int i = 0; i < num_methods; i++) {
        jobject meth = JJ(GetObjectArrayElement, methods, i);
        jobject name_o = JJ(CallObjectMethod, meth, Method_toString_mid);
        const char *decl = JJ(GetStringUTFChars, name_o, NULL);
        const char *arg_types = strchr(decl, '(') + 1;
        //const char *arg_types_end = strchr(arg_types, ')');
//        printf("method[%d]=%p %s\n", i, meth, decl);

        const char *meth_name = NULL;
        const char *ret_type = NULL;
        if (!is_constr) {
            meth_name = strprev(arg_types, '.') + 1;
            ret_type = strprev(meth_name, ' ') - 1;
            ret_type = strprev(ret_type, ' ') + 1;

            int name_len = strlen(name);
            if (strncmp(name, meth_name, name_len/*arg_types - meth_name - 1*/) || meth_name[name_len] != '('/*(*/) {
                continue;
            }
        }
//        printf("method[%d]=%p %s\n", i, meth, decl);
//        printf("!!!%s\n", arg_types);
//        printf("name=%p meth_name=%s\n", name, meth_name);

        bool found = true;
        for (int i = 0; i < n_args; i++) {
            if (!py2jvalue(&arg_types, args[i], &jargs[i])) {
                goto next_method;
            }

            if (*arg_types == ',') {
                arg_types++;
            }
        }

        if (found) {
//            printf("found!\n");
            jmethodID method_id = JJ(FromReflectedMethod, meth);
            jobject res;
            if (is_constr) {
                res = JJ(NewObjectA, obj, method_id, jargs);
                JJ(ReleaseStringUTFChars, name_o, decl);
                return new_jobject(res);
            } else {
                res = JJ(CallObjectMethodA, obj, method_id, jargs);
                mp_obj_t ret = jvalue2py(ret_type, res);
                JJ(ReleaseStringUTFChars, name_o, decl);
                if (ret != MP_OBJ_NULL) {
                    return ret;
                }
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "cannot handle return type"));
            }
        }

next_method:
        JJ(ReleaseStringUTFChars, name_o, decl);
    }

    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "method not found"));
}


STATIC mp_obj_t jmethod_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    if (n_kw != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "kwargs not supported"));
    }
    mp_obj_jmethod_t *self = self_in;

    const char *name = qstr_str(self->name);
//    jstring meth_name = JJ(NewStringUTF, name);

    jclass obj_class = self->obj;
    if (!self->is_static) {
        obj_class = JJ(GetObjectClass, self->obj);
    }
    jarray methods = JJ(CallObjectMethod, obj_class, Class_getMethods_mid);

    return call_method(self->obj, name, methods, false, n_args, args);
}

STATIC const mp_obj_type_t jmethod_type = {
    { &mp_type_type },
    .name = MP_QSTR_jmethod,
    .print = jmethod_print,
    .call = jmethod_call,
//    .attr = jobject_attr,
//    .locals_dict = (mp_obj_t)&jobject_locals_dict,
};

#ifdef __ANDROID__
#define LIBJVM_SO "libdvm.so"
#else
#define LIBJVM_SO "libjvm.so"
#endif

STATIC void create_jvm() {
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
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "unable to load libjvm.so, use LD_LIBRARY_PATH"));
    }
    int (*_JNI_CreateJavaVM)(void*, void**, void*) = dlsym(libjvm, "JNI_CreateJavaVM");

    int st = _JNI_CreateJavaVM(&jvm, (void**)&env, &args);
    if (st < 0 || !env) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "unable to create JVM"));
    }

    Class_class = JJ(FindClass, "java/lang/Class");
    jclass method_class = JJ(FindClass, "java/lang/reflect/Method");
    String_class = JJ(FindClass, "java/lang/String");

    Class_getField_mid = (*env)->GetMethodID(env, Class_class, "getField",
                                     "(Ljava/lang/String;)Ljava/lang/reflect/Field;");
    Class_getMethods_mid = (*env)->GetMethodID(env, Class_class, "getMethods",
                                     "()[Ljava/lang/reflect/Method;");
    Class_getConstructors_mid = (*env)->GetMethodID(env, Class_class, "getConstructors",
                                     "()[Ljava/lang/reflect/Constructor;");
    Method_getName_mid = (*env)->GetMethodID(env, method_class, "getName",
                                     "()Ljava/lang/String;");
    Method_toString_mid = (*env)->GetMethodID(env, method_class, "toString",
                                     "()Ljava/lang/String;");

    List_class = JJ(FindClass, "java/util/List");
    List_get_mid = JJ(GetMethodID, List_class, "get",
                                     "(I)Ljava/lang/Object;");
    List_set_mid = JJ(GetMethodID, List_class, "set",
                                     "(ILjava/lang/Object;)Ljava/lang/Object;");
}

STATIC mp_obj_t mod_jni_cls(mp_obj_t cls_name_in) {
    const char *cls_name = mp_obj_str_get_str(cls_name_in);
    if (!env) {
        create_jvm();
    }
    jclass cls = JJ(FindClass, cls_name);

    mp_obj_jclass_t *o = m_new_obj(mp_obj_jclass_t);
    o->base.type = &jclass_type;
    o->cls = cls;
    return o;
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_jni_cls_obj, mod_jni_cls);

STATIC mp_obj_t mod_jni_env() {
    return mp_obj_new_int((mp_int_t)env);
}
MP_DEFINE_CONST_FUN_OBJ_0(mod_jni_env_obj, mod_jni_env);

STATIC const mp_map_elem_t mp_module_jni_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_jni) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_cls), (mp_obj_t)&mod_jni_cls_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_env), (mp_obj_t)&mod_jni_env_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_jni_globals, mp_module_jni_globals_table);

const mp_obj_module_t mp_module_jni = {
    .base = { &mp_type_module },
    .name = MP_QSTR_jni,
    .globals = (mp_obj_dict_t*)&mp_module_jni_globals,
};
