// CPython module for MicroPython
// Allows to run MicroPython code from CPython
// and to package this easily for pip/PyPi

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// From libmicropython.a
extern "C" {
    int micropython_unix_main(int argc, char **argv);
}

namespace py = pybind11;

void convert_to_argv(const std::vector<std::string>& args, int& argc, char*** argv) {
    argc = args.size();
    *argv = new char*[argc + 1];

    int i = 0;
    for (const auto& s : args) {
        (*argv)[i++] = strdup(s.c_str());
    }
    (*argv)[argc] = nullptr;
}

void free_argv(int argc, char** argv) {
    for (int i = 0; i < argc; ++i) {
        free(argv[i]);
    }
    delete[] argv;
}

int run_main(std::vector<std::string> args) {

    int argc;
    char** argv;
    convert_to_argv(args, argc, &argv);

    const int ret = micropython_unix_main(argc, argv);

    free_argv(argc, argv);
    return ret;
}

PYBIND11_MODULE(micropython_module, m) {
    m.doc() = "CPython module for MicroPython";
    m.def("run_main", &run_main, "Run main()");
}
