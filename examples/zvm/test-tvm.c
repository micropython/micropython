//   Copyright (C) 2018 TASChain
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "py/tvm.h"

void test_execute() {
    tvm_start();

    for (int i = 0; i < 1; i++) {
        tvm_set_gas(1000000000);

        const char *str = "class A:\n"
                          "\n"
                          "    @register.public(int)\n"
                          "    def test(self,a):\n"
                          "        return \"hello\"\n"
                          "    def test1(self,a):\n"
                          "        return 55555\n"
                          "    def test2(self,a):\n"
                          "        return True\n"
                          "\n";


        tvm_execute_result_t result;
        tvm_init_result(&result);
        // 返回值 str
        tvm_execute("A().test(1)", "A", PARSE_KIND_EVAL, &result);
        tvm_print_result(&result);
        assert(strcmp(result.content, "hello") == 0);
        tvm_deinit_result(&result);

        // 返回值 int
        tvm_execute("A().test1(1)", "A", PARSE_KIND_EVAL, &result);
        tvm_print_result(&result);
        assert(strcmp(result.content, "55555") == 0);
        tvm_deinit_result(&result);

        // 返回值 bool
        tvm_execute("A().test2(1)", "A", PARSE_KIND_EVAL, &result);
        tvm_print_result(&result);
        assert(strcmp(result.content, "1") == 0);
        tvm_deinit_result(&result);

        // 错误返回
        tvm_execute("B().test(1)", "A", PARSE_KIND_EVAL, &result);
        tvm_print_result(&result);
        assert(result.result_type == RETURN_TYPE_EXCEPTION);
        tvm_deinit_result(&result);
    }
}

void test_gc() {
    for (int i = 0; i < 10000; i++) {

        tvm_start();
        tvm_set_gas(100000);

        tvm_create_context();

        test_execute();

        tvm_remove_context();

        tvm_gc();

    }
}

void contract_call_callback (const char *contractAddr, const char *funName, const char *JSON, tvm_execute_result_t *result) {
    assert(result);
    if (strcmp(JSON, "[\"1\"]") == 0) {
        result->result_type = RETURN_TYPE_STRING;
        result->content = malloc(100);
        memset(result->content, 0, 100);
        memcpy(result->content, "Hello", 5);
    } else if (strcmp(JSON, "[\"2\"]") == 0) {
        result->result_type = RETURN_TYPE_INT;
        result->content = malloc(100);
        memset(result->content, 0, 100);
        memcpy(result->content, "55555", 5);
    }
    else if (strcmp(JSON, "[\"3\"]") == 0) {
        result->result_type = RETURN_TYPE_NONE;
    }
    else if (strcmp(JSON, "[\"4\"]") == 0) {
        result->result_type = RETURN_TYPE_BOOL;
        result->content = malloc(100);
        memset(result->content, 0, 100);
        memcpy(result->content, "0", 1);
    }
    else if (strcmp(JSON, "[\"5\"]") == 0) {
        result->result_type = RETURN_TYPE_EXCEPTION;
        result->content = malloc(100);
        memset(result->content, 0, 100);
        memcpy(result->content, "World", 5);
    }
}
void test_contract_call() {

    tvm_start();
    tvm_set_gas(1000000);

    contract_call_fn = &contract_call_callback;

    const char *str = "\n"
                      "t = Contract('A').B('1')\n"
                      "print(t)\n"
                      "t = Contract('A').B('2')\n"
                      "print(t)\n"
                      "t = Contract('A').B('3')\n"
                      "print(t)\n"
                      "t = Contract('A').B('4')\n"
                      "print(t)\n"
//                      "t = Contract('A').B('5')\n"
//                      "print(t)\n"
//                      "t = Contract('A').B(Contract)\n"
//                      "print(t)\n"
//                      "t = Contract('A').B([1,2,3,4])\n"
//                      "print(t)\n"
//                      "t = Contract('A').B({'a':1})\n"
//                      "print(t)\n"
//                      "t = Contract('A').B(a=1)\n"
//                      "print(t)\n"
                      "print('Test Finished')\n"
                      "\n";

    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(str, "Testcontract_call", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    assert(result.result_type != RETURN_TYPE_EXCEPTION);
    tvm_deinit_result(&result);

}

void test_lib_path() {
    tvm_start();
    tvm_set_lib_path("../py");

    const char *str = "import coin\n"
                      "print(coin.TAS)"
                      "\n";

    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(str, "Testcontract_call", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);
}

void test_malloc_crash() {
    tvm_set_gas(0);

    tvm_start();

    const char *str = "\n"
                      "a = 'hello'\n"
                      "print(a)\n"
                      "b = 'world'\n"
                      "print(b)\n"
                      "\n";

    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(str, "Testcontract_call", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);
}

void test_gas() {
    tvm_start();

    tvm_set_gas(1000);

    const char *str = "\n"
                      "a = 'hello'\n"
                      "print(a)\n"
                      "b = 'world'\n"
                      "print(b)\n"
                      "print(1123000)\n"
                      "print(1123000)\n"
                      "print(1123000)\n"
                      "print(1123000)\n"
                      "print(1123000)\n"
                      "print(1123000)\n"
                      "print(1230000)\n"
                      "print(1123000)\n"
                      "print(1123000)\n"
                      "\n";


    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(str, "Testcontract_call", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

    tvm_gas_report();
}

void exec_pycode_exception(const char* pycode) {
    tvm_start();

    tvm_set_gas(1000);

    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(pycode, "test_float", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    assert(result.result_type == RETURN_TYPE_EXCEPTION);
    tvm_deinit_result(&result);
}

void test_not_supported() {
    // float
    const char *str = "\n"
                      "a = 0.123\n"
                      "\n";
    exec_pycode_exception(str);
    str = "\n"
          "a = 5 / 3\n"
          "print(a)\n";
    exec_pycode_exception(str);
    str = "\n"
          "a = 123-12j\n";
    exec_pycode_exception(str);

    // open file
    str = "\n"
          "open(file, mode='r')\n"
          "\n";
    exec_pycode_exception(str);

    // socket
    str = "\n"
          "import socket\n"
          "\n";
    exec_pycode_exception(str);

    // eval
    str = "\n"
          "eval('0+1')\n"
          "\n";
    exec_pycode_exception(str);

    // exec
    str = "\n"
          "exec('print(\"test\")')\n"
          "\n";
    exec_pycode_exception(str);
}

void test_register() {
    tvm_start();
    tvm_set_gas(10000000);

    const char *str = "class Token():\n"
                      "\n"
                      "    def __init__(self):\n"
                      "        print('init')\n"
                      "\n"
                      "    @register.public()\n"
                      "    def myprint(self):\n"
                      "        print(self)\n"
                      "        print('i am Token')\n"
                      "\n"
                      "    @register.public(str)\n"
                      "    def myprint2(self, a):\n"
                      "        print(self)\n"
                      "        print('i am Token', a)\n"
                      "    @register.public(str, str, str, str, str, str, str, str)\n"
                      "    def myprint4(self, a, b, c, d, e, f, g, h):\n"
                      "        print(self)\n"
                      "        print('i am Token', a)\n"
                      "\n"
//                      "    @register.public\n"
//                      "    def myprint3(self, a):\n"
//                      "        print(self)\n"
//                      "        print('i am Token', a)\n"
                      "\n"
                      "token = Token()\n"
                      "token.myprint()\n"
                      "token.myprint2(2)\n"
//                      "token.myprint3(2)\n"
                      "\n";

    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(str, "test_register", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

    tvm_init_result(&result);
    tvm_fun_call("Token", "myprint2", "[2]", &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

    tvm_init_result(&result);
    tvm_fun_call("Token", "myprint4", "[\"1\", \"1\",\"1\",\"1\",\"1\",\"1\",\"1\",\"1\"]", &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

}

void test_1() {
    tvm_start();
    tvm_set_gas(500000);

    const char *str = "\n"
                      "class Max():\n"
                      "    def __init__(self):\n"
                      "        pass\n"
                      "\n"
                      "    def exec(self, max):\n"
                      "        counter = \"\"\n"
                      "        while 0 <= max:\n"
                      "            counter += str(max)\n"
                      "            max -= 1\n"
                      "\n"
                      "m = Max()\n"
                      "m.exec(1000000)\n"
                      "\n";

    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(str, "test_1", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

    tvm_gas_report();

    tvm_delete();
}


// storage
void storage_get_data(const char* key, int len, char** out_value, int* out_len) {

}

void storage_set_data (const char* key, int len, const char* value, int value_len) {
    printf("storage_set_data: key: %s", key);
//    for (int i = 0; i < len; ++i) {
//        printf("%x", key[i]);
//    }
    printf(" value: ");
    for (int i = 0; i < value_len; ++i) {
        printf("%x", value[i]);
    }
    printf("\n");
}

void storage_remove_data (const char* key, int len) {
    printf("storage_remove_data: key: %s", key);
}

void test_storage() {
    storage_get_data_fn = storage_get_data;
    storage_set_data_fn = storage_set_data;

    tvm_start();
    tvm_set_gas(10000000);

    const char *str = "class Token():\n"
                      "\n"
                      "    def __init__(self):\n"
                      "        print('init')\n"
                      "        print(Token.__setattr__)\n"
//                      "        self.text = 'hello_world'\n"
                      "\n"
                      "    def deploy(self, a):\n"
                      "        print(self.text)\n"
                      "        print(a)\n"
                      "\n"
                      "\n";

    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(str, "test_storage", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

    tvm_init_result(&result);
    tvm_fun_call("Token", "deploy", "hello", &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

}

void test_storage2() {
    storage_get_data_fn = storage_get_data;
    storage_set_data_fn = storage_set_data;
    storage_remove_data_fn = storage_remove_data;

    tvm_start();
    tvm_set_gas(10000000);

    const char *str = "class Token():\n"
                      "\n"
                      "    def __init__(self):\n"
                      "        del self.foo\n"
                      "        self.int = 2147483647\n"
                      "        self.bigint = 10000000000000000000000000000000\n"
                      "        self.str = 'hello'\n"
                      "        self.bool = True\n"
                      "        self.bool = False\n"
                      "        self.none = None\n";

    tvm_set_register();

    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(str, "test_storage", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

    tvm_init_result(&result);
    tvm_fun_call("Token", "__init__", "[]", &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

}

void test_zdict() {
    storage_get_data_fn = storage_get_data;
    storage_set_data_fn = storage_set_data;

    tvm_start();
    tvm_set_gas(10000000);

    const char *str = "print(zdict)\n"
                      "data = zdict()\n"
//                      "print('text' in data)\n"
                      "\n"
                      "data['text'] = 'hello world'\n"
//                      "print('text' in data)\n"
                      "print(data['text'])\n"
                      "\n"
                      "\n";

    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(str, "test_zdict", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);
}

void test_tvm_abli_call() {
    tvm_start();
    tvm_set_gas(10000000);

    const char *str =
                      "class Token():\n"
                      "\n"
                      "    def __init__(self):\n"
                      "        print(self)\n"
                      "        print('init')\n"
                      "\n"
                      "    @register.public()\n"
                      "    def deploy(self):\n"
                      "        print(self)\n"
                      "        print('deploy')\n"
                      "\n"
                      "    def foo(self):\n"
                      "        print(xxxx)\n"
                      "\n"
                      "    @register.public(str, str)\n"
                      "    def deploy2(self, a, b):\n"
                      "        print('deploy2')\n"
                      "        print(a)\n"
                      "        print(b)\n"
                      "        self.foo()\n"
                      "\n"
                      "    def deploy3(self):\n"
                      "        make_error\n"
                      ""
                      "\n";

    tvm_set_register();
    tvm_set_msg("zvxxx", 500);

    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(str, "test_tvm_abli_call", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

    tvm_init_result(&result);
    tvm_fun_call("Token", "__init__", NULL, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

    tvm_init_result(&result);
    tvm_fun_call("Token", "deploy", NULL, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

    tvm_init_result(&result);
    tvm_fun_call("Token", "deploy2", "[\"hello\", \"world\"]", &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

    tvm_init_result(&result);
    tvm_fun_call("Token", "deploy3", NULL, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

}

void test_msg() {
    tvm_start();
    tvm_set_gas(10000000);

    const char *str = "print(msg)\n"
                      "print(msg.sender)\n"
                      "print(msg.value)\n";

    tvm_set_msg("zvxxx", 100);

    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(str, "test_msg", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);
}

void test_2() {
    for (int i = 0; i < 100; ++i) {
        test_1();
        tvm_delete();
    }
}

int main() {
//    test_execute();

//    test_gc();

//    test_contract_call();

//    test_lib_path();

//    test_malloc_crash();

//    test_gas();

//    test_not_supported();

//    test_register();

//    test_storage();
//    test_storage2();

//    test_zdict();

    test_tvm_abli_call();

//    test_msg();

//    test_1();

//    test_2();


    printf("finished\n");
}

