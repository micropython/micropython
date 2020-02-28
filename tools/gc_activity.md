# GC Analysis

Here are some terse instructions on doing heap analysis via gdb.

First, build your port with `LOG_HEAP_ACTIVITY` defined and load it onto your
board. This will enable calls to a gc log function that isn't inlined so it uses
only one breakpoint.

Next, double check that the `break gc.c:110` line  in `output_gc_until_repl.txt`
has the correct line number. Also make sure the tar ext line connects to the
correct port. GDB is usually :3333 and JLink is :2331.

Now, run gdb from your port directory:

```
arm-none-eabi-gdb -x ../../tools/output_gc_until_repl.txt build-metro_m0_express/firmware.elf
```

This will take a little time while it breaks, backtraces and continues for every
gc change until the repl. At the end it will have a `mylog.txt` file.

To analyze that file run:
```
python3 ../tools/gc_activity.py mylog.txt
```

It will create a file `allocation_history.json` which documents the lifespan of
the allocations. It also outputs a tree of the current number of blocks
allocated by a piece of code such as:
```
main.c:298 main 5 blocks
 main.c:159 reset_mp 2 blocks
   ../py/runtime.c:81 mp_init 2 blocks
     ../py/objmodule.c:227 mp_module_init 2 blocks
       ../py/objdict.c:595 mp_obj_dict_init 2 blocks
         ../py/map.c:79 mp_map_init 2 blocks
 main.c:160 reset_mp 1 blocks
   ../py/objlist.c:458 mp_obj_list_init 1 blocks
 main.c:164 reset_mp 1 blocks
   ../py/objlist.c:458 mp_obj_list_init 1 blocks
 main.c:166 reset_mp 1 blocks
   ../py/objexcept.c:297 mp_obj_new_exception 1 blocks
     ../py/objexcept.c:307 mp_obj_new_exception_args 1 blocks
       ../py/objexcept.c:130 mp_obj_exception_make_new 1 blocks
main.c:311 main 289 blocks
 main.c:211 start_mp 14 blocks
   main.c:196 maybe_run 14 blocks
     ../lib/utils/pyexec.c:501 pyexec_file 14 blocks
       main.c:352 mp_lexer_new_from_file 14 blocks
         ../py/../extmod/vfs_fat_lexer.c:80 fat_vfs_lexer_new_from_file 14 blocks
           ../py/qstr.c:184 qstr_from_str 14 blocks
             ../py/qstr.c:216 qstr_from_strn 8 blocks
             ../py/qstr.c:240 qstr_from_strn 6 blocks
               ../py/qstr.c:146 qstr_add 6 blocks
 main.c:220 start_mp 275 blocks
   main.c:196 maybe_run 275 blocks
     ../lib/utils/pyexec.c:508 pyexec_file 275 blocks
       ../lib/utils/pyexec.c:82 parse_compile_execute 4 blocks
         ../py/compile.c:3511 mp_compile 3 blocks
           ../py/compile.c:3451 mp_compile_to_raw_code 3 blocks
             ../py/compile.c:3092 compile_scope 3 blocks
               ../py/emitbc.c:419 mp_emit_bc_end_pass 3 blocks
         ../py/compile.c:3513 mp_compile 1 blocks
           ../py/emitglue.c:131 mp_make_function_from_raw_code 1 blocks
             ../py/objfun.c:364 mp_obj_new_fun_bc 1 blocks
       ../lib/utils/pyexec.c:88 parse_compile_execute 271 blocks
         ../py/runtime.c:551 mp_call_function_0 271 blocks
           ../py/runtime.c:577 mp_call_function_n_kw 271 blocks
             ../py/objfun.c:275 fun_bc_call 271 blocks
               ../py/vm.c:1161 mp_execute_bytecode 269 blocks
                 ../py/runtime.c:1315 mp_import_name 269 blocks
                   ../py/builtinimport.c:442 mp_builtin___import__ 2 blocks
                     ../py/objmodule.c:113 mp_obj_new_module 1 blocks
                     ../py/objmodule.c:115 mp_obj_new_module 1 blocks
                       ../py/objdict.c:599 mp_obj_new_dict 1 blocks
                   ../py/builtinimport.c:473 mp_builtin___import__ 267 blocks
                     ../py/builtinimport.c:230 do_load 140 blocks
                       ../py/emitglue.c:469 mp_raw_code_load_file 140 blocks
                         ../py/emitglue.c:356 mp_raw_code_load 140 blocks
                           ../py/emitglue.c:320 load_raw_code 8 blocks
                             ../py/emitglue.c:295 load_bytecode_qstrs 8 blocks
                               ../py/emitglue.c:265 load_qstr 8 blocks
                                 ../py/qstr.c:216 qstr_from_strn 8 blocks
                           ../py/emitglue.c:331 load_raw_code 2 blocks
                             ../py/emitglue.c:277 load_obj 1 blocks
                               ../py/vstr.c:53 vstr_init_len 1 blocks
                                 ../py/vstr.c:46 vstr_init 1 blocks
                             ../py/emitglue.c:280 load_obj 1 blocks
                               ../py/objstr.c:2001 mp_obj_new_str_from_vstr 1 blocks
                           ../py/emitglue.c:334 load_raw_code 130 blocks
                             ../py/emitglue.c:306 load_raw_code 6 blocks
                             ../py/emitglue.c:325 load_raw_code 4 blocks
                             ../py/emitglue.c:328 load_raw_code 19 blocks
                               ../py/emitglue.c:265 load_qstr 19 blocks
                                 ../py/qstr.c:216 qstr_from_strn 8 blocks
                                 ../py/qstr.c:240 qstr_from_strn 11 blocks
                                   ../py/qstr.c:146 qstr_add 11 blocks
                             ../py/emitglue.c:334 load_raw_code 101 blocks
                               ../py/emitglue.c:306 load_raw_code 80 blocks
                               ../py/emitglue.c:320 load_raw_code 8 blocks
                                 ../py/emitglue.c:295 load_bytecode_qstrs 8 blocks
                                   ../py/emitglue.c:265 load_qstr 8 blocks
                                     ../py/qstr.c:216 qstr_from_strn 8 blocks
                               ../py/emitglue.c:325 load_raw_code 13 blocks
                     ../py/builtinimport.c:231 do_load 127 blocks
                       ../py/builtinimport.c:181 do_execute_raw_code 127 blocks
                         ../py/runtime.c:551 mp_call_function_0 127 blocks
                           ../py/runtime.c:577 mp_call_function_n_kw 127 blocks
                             ../py/objfun.c:275 fun_bc_call 127 blocks
                               ../py/vm.c:400 mp_execute_bytecode 6 blocks
                                 ../py/runtime.c:168 mp_store_name 6 blocks
                                   ../py/objdict.c:612 mp_obj_dict_store 6 blocks
                                     ../py/map.c:283 mp_map_lookup 6 blocks
                                       ../py/map.c:130 mp_map_rehash 6 blocks
                               ../py/vm.c:847 mp_execute_bytecode 2 blocks
                                 ../py/emitglue.c:131 mp_make_function_from_raw_code 2 blocks
                                   ../py/objfun.c:364 mp_obj_new_fun_bc 2 blocks
                               ../py/vm.c:855 mp_execute_bytecode 3 blocks
                                 ../py/emitglue.c:131 mp_make_function_from_raw_code 3 blocks
                                   ../py/objfun.c:364 mp_obj_new_fun_bc 3 blocks
                               ../py/vm.c:903 mp_execute_bytecode 110 blocks
                                 ../py/runtime.c:577 mp_call_function_n_kw 110 blocks
                                   ../py/objfun.c:129 fun_builtin_var_call 110 blocks
                                     ../py/modbuiltins.c:56 mp_builtin___build_class__ 5 blocks
                                       ../py/objdict.c:599 mp_obj_new_dict 5 blocks
                                     ../py/modbuiltins.c:60 mp_builtin___build_class__ 84 blocks
                                       ../py/runtime.c:551 mp_call_function_0 84 blocks
                                         ../py/runtime.c:577 mp_call_function_n_kw 84 blocks
                                           ../py/objfun.c:275 fun_bc_call 84 blocks
                                             ../py/vm.c:400 mp_execute_bytecode 17 blocks
                                               ../py/runtime.c:168 mp_store_name 17 blocks
                                                 ../py/objdict.c:612 mp_obj_dict_store 17 blocks
                                                   ../py/map.c:283 mp_map_lookup 17 blocks
                                                     ../py/map.c:130 mp_map_rehash 17 blocks
                                             ../py/vm.c:847 mp_execute_bytecode 9 blocks
                                               ../py/emitglue.c:131 mp_make_function_from_raw_code 9 blocks
                                                 ../py/objfun.c:364 mp_obj_new_fun_bc 9 blocks
                                             ../py/vm.c:855 mp_execute_bytecode 8 blocks
                                               ../py/emitglue.c:131 mp_make_function_from_raw_code 8 blocks
                                                 ../py/objfun.c:364 mp_obj_new_fun_bc 8 blocks
                                             ../py/vm.c:903 mp_execute_bytecode 50 blocks
                                               ../py/runtime.c:577 mp_call_function_n_kw 50 blocks
                                                 ../py/objboundmeth.c:69 bound_meth_call 1 blocks
                                                   ../py/runtime.c:577 mp_call_function_n_kw 1 blocks
                                                     ../py/objfun.c:86 fun_builtin_2_call 1 blocks
                                                       ../py/objproperty.c:66 property_setter 1 blocks
                                                 ../py/objtype.c:816 type_call 49 blocks
                                                   ../py/objtype.c:248 mp_obj_instance_make_new 7 blocks
                                                     ../py/objtype.c:52 mp_obj_new_instance 7 blocks
                                                   ../py/objtype.c:312 mp_obj_instance_make_new 42 blocks
                                                     ../py/runtime.c:594 mp_call_method_n_kw 42 blocks
                                                       ../py/runtime.c:577 mp_call_function_n_kw 42 blocks
                                                         ../py/objfun.c:275 fun_bc_call 42 blocks
                                                           ../py/vm.c:381 mp_execute_bytecode 14 blocks
                                                             ../py/obj.c:442 mp_obj_subscr 14 blocks
                                                               ../py/objarray.c:461 array_subscr 14 blocks
                                                           ../py/vm.c:415 mp_execute_bytecode 14 blocks
                                                             ../py/runtime.c:1083 mp_store_attr 14 blocks
                                                               ../py/objtype.c:645 mp_obj_instance_attr 14 blocks
                                                                 ../py/objtype.c:636 mp_obj_instance_store_attr 14 blocks
                                                                   ../py/map.c:283 mp_map_lookup 14 blocks
                                                                     ../py/map.c:130 mp_map_rehash 14 blocks
                                                           ../py/vm.c:903 mp_execute_bytecode 14 blocks
                                                             ../py/runtime.c:577 mp_call_function_n_kw 14 blocks
                                                               ../py/objtype.c:816 type_call 14 blocks
                                                                 ../py/objarray.c:192 bytearray_make_new 14 blocks
                                                                   ../py/objarray.c:100 array_new 7 blocks
                                                                   ../py/objarray.c:111 array_new 7 blocks
                                     ../py/modbuiltins.c:80 mp_builtin___build_class__ 1 blocks
                                       ../py/objtuple.c:242 mp_obj_new_tuple 1 blocks
                                     ../py/modbuiltins.c:82 mp_builtin___build_class__ 20 blocks
                                       ../py/runtime.c:577 mp_call_function_n_kw 20 blocks
                                         ../py/objtype.c:816 type_call 20 blocks
                                           ../py/objtype.c:794 type_make_new 20 blocks
                                             ../py/objtype.c:904 mp_obj_new_type 20 blocks
                               ../py/vm.c:974 mp_execute_bytecode 6 blocks
                                 ../py/runtime.c:594 mp_call_method_n_kw 6 blocks
                                   ../py/runtime.c:577 mp_call_function_n_kw 6 blocks
                                     ../py/objfun.c:86 fun_builtin_2_call 6 blocks
                                       ../py/objnamedtuple.c:169 new_namedtuple_type 6 blocks
                                         ../py/objnamedtuple.c:140 mp_obj_new_namedtuple_type 6 blocks
               ../py/vm.c:400 mp_execute_bytecode 2 blocks
                 ../py/runtime.c:168 mp_store_name 2 blocks
                   ../py/objdict.c:612 mp_obj_dict_store 2 blocks
                     ../py/map.c:283 mp_map_lookup 2 blocks
                       ../py/map.c:130 mp_map_rehash 2 blocks
main.c:321 main 2 blocks
 ../lib/utils/pyexec.c:374 pyexec_friendly_repl 2 blocks
   ../py/vstr.c:46 vstr_init 2 blocks
296 total blocks
```

To load the json file in the browser install http-server with npm
(`npm install -g http-server`) and run it from the `micropython` directory with
`http-server`.

Navigate to `https://127.0.0.1/tools/gc_activity.html` to see the data. (You
will likely need to edit the html to point to the right place for your file.)
