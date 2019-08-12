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

        char *pycode =
                "\n"
                "class Register(object):\n"
                "\n"
                "    def __init__(self):\n"
                "        self.funcinfo = {}\n"
                "        self.abiinfo = []\n"
                "\n"
                "    def public(self , *dargs):\n"
                "        def wrapper(func):\n"
                "            paranametuple = func.__para__\n"
                "            paraname = list(paranametuple)\n"
                "            paraname.remove(\"self\")\n"
                "            paratype = []\n"
                "            for i in range(len(paraname)):\n"
                "                paratype.append(dargs[i])\n"
                "            self.funcinfo[func.__name__] = [paraname,paratype]\n"
                "            tmp = {}\n"
                "            tmp[\"FuncName\"] = func.__name__\n"
                "            tmp[\"Args\"] = paratype\n"
                "            self.abiinfo.append(tmp)\n"
                "            abiexport(str(self.abiinfo))\n"
                "            \n"
                "            def _wrapper(*args , **kargs):\n"
                "                return func(*args, **kargs)\n"
                "            return _wrapper\n"
                "        return wrapper\n"
                "\n"
                "import builtins\n"
                "builtins.register = Register()";

        tvm_execute_result_t result;
        tvm_init_result(&result);

        tvm_execute(pycode, "A", PARSE_KIND_FILE, &result);
        tvm_print_result(&result);
        assert(result.result_type != RETURN_TYPE_EXCEPTION);
        tvm_deinit_result(&result);

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


        // 导出ABI
        tvm_execute(str, "A", PARSE_KIND_FILE, &result);
        tvm_print_result(&result);
        assert(result.result_type != RETURN_TYPE_EXCEPTION);
        tvm_deinit_result(&result);


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

void contract_call_callback (const char *contractAddr, const char *contractName, const char *contractArgs, tvm_execute_result_t *result) {
    assert(result);
    if (strcmp(contractArgs, "1") == 0) {
        result->result_type = RETURN_TYPE_STRING;
        result->content = malloc(100);
        memset(result->content, 0, 100);
        memcpy(result->content, "Hello", 5);
    } else if (strcmp(contractArgs, "2") == 0) {
        result->result_type = RETURN_TYPE_INT;
        result->content = malloc(100);
        memset(result->content, 0, 100);
        memcpy(result->content, "55555", 5);
    }
    else if (strcmp(contractArgs, "3") == 0) {
        result->result_type = RETURN_TYPE_NONE;
    }
    else if (strcmp(contractArgs, "4") == 0) {
        result->result_type = RETURN_TYPE_BOOL;
        result->content = malloc(100);
        memset(result->content, 0, 100);
        memcpy(result->content, "0", 1);
    }
    else if (strcmp(contractArgs, "5") == 0) {
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

    const char *str = "import account\n"
                      "\n"
                      "t = account.contract_call('A', 'B', '1')\n"
                      "print(t)\n"
                      "t = account.contract_call('A', 'B', '2')\n"
                      "print(t)\n"
                      "t = account.contract_call('A', 'B', '3')\n"
                      "print(t)\n"
                      "t = account.contract_call('A', 'B', '4')\n"
                      "print(t)\n"
//                      "t = account.contract_call('A', 'B', '5')\n"
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

void test_lib_line() {
    tvm_start();
    //TODO line number(+1) error
    tvm_set_lib_line(4);
    tvm_set_gas(1000);

    const char *str = "\n"
                      "\n"
                      "\n"
                      "\n"
                      "a = 0.123\n"
                      "\n";

    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(str, "test_float", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    assert(result.result_type == RETURN_TYPE_EXCEPTION);
    tvm_deinit_result(&result);
}

void test_register() {
    tvm_start();
    tvm_set_gas(10000000);

    const char *str = "print(register.public)\n"
                      "print(type(register.public()))\n"
                      "class Token():\n"
                      "\n"
                      "    def __init__(self):\n"
                      "        print('init')\n"
                      "\n"
                      "    @register.public()\n"
                      "    def myprint(self):\n"
                      "        print(self)\n"
                      "        print('i am Token')\n"
                      "\n"
                      "    @register.public(int)\n"
                      "    def myprint2(self, a):\n"
                      "        print(self)\n"
                      "        print('i am Token', a)\n"
                      "\n"
//                      "    @register.public\n"
//                      "    def myprint3(self, a):\n"
//                      "        print(self)\n"
//                      "        print('i am Token', a)\n"
                      "\n"
//                      "token = Token()\n"
//                      "token.myprint()\n"
//                      "token.myprint2(2)\n"
//                      "token.myprint3(2)\n"
                      "\n";

    tvm_execute_result_t result;
    tvm_init_result(&result);
    tvm_execute(str, "test_register", PARSE_KIND_FILE, &result);
    tvm_print_result(&result);
    tvm_deinit_result(&result);

    tvm_abi_call("Token", "myprint2", "hello");

}

int main() {
//    test_execute();

//    test_gc();

//    test_contract_call();

//    test_lib_path();

//    test_malloc_crash();

//    test_gas();

//    test_not_supported();

//    test_lib_line();

    test_register();

    printf("finished\n");
}

