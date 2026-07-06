"""
This script processes the output from the C preprocessor and extracts all
qstr. Each qstr is transformed into a qstr definition of the form 'Q(...)'.

This script works with Python 3.3+.

Example call for command 'split':

    makeqstrdefs.py \
        split \
        qstr \
        micropython/ports/rp2/build-RPI_PICO_W/genhdr/qstr.i.last \
        micropython/ports/rp2/build-RPI_PICO_W/genhdr/qstr \
        _


Example call for command 'pp':

    makeqstrdefs.py \
        pp       /usr/bin/arm-none-eabi-gcc -E \
        output   micropython/ports/rp2/build-RPI_PICO_W/genhdr/qstr.i.last \
        cflags   -Imicropython/ports/rp2/lwip_inc -DMICROPY_PY_LWIP=1 -mcpu=cortex-m0plus \
        cxxflags -Imicropython/ports/rp2/lwip_inc -mcpu=cortex-m0plus -mthumb -DNO_QSTR \
        sources  micropython/py/argcheck.c micropython/py/builtinhelp.c

    In the folder specified by the 'output' parameter, a file 'mp_sys_version.json' is
    written with this example content:
        {
            "sys_implementation_build": "RPI_PICO_W",
            "sys_implementation_machine": "Raspberry Pi Pico W with RP2040",
            "sys_version": "3.4.0; MicroPython v1.23.0-preview.3247.g964803ab74.dirty on 2026-07-03"
        }
    
    This allows to verify if the running MicroPython version corresponds to this build:
        MicroPython:
        >>> sys.version                   # Must correspond to mp_sys_version.json: sys_version
        >>> sys.implementation._machine   # Must correspond to mp_sys_version.json: sys_implementation_machine
"""

import io
import os
import re
import subprocess
import sys
import json
import multiprocessing, multiprocessing.dummy


# Extract MP_QSTR_FOO macros.
_MODE_QSTR = "qstr"

# Extract MP_COMPRESSED_ROM_TEXT("") macros.  (Which come from MP_ERROR_TEXT)
_MODE_COMPRESS = "compress"

# Extract MP_REGISTER_(EXTENSIBLE_)MODULE(...) macros.
_MODE_MODULE = "module"

# Extract MP_REGISTER_ROOT_POINTER(...) macros.
_MODE_ROOT_POINTER = "root_pointer"


class PreprocessorError(Exception):
    pass


def is_c_source(fname):
    return os.path.splitext(fname)[1] in [".c"]


def is_cxx_source(fname):
    return os.path.splitext(fname)[1] in [".cc", ".cp", ".cxx", ".cpp", ".CPP", ".c++", ".C"]


def write_version_json(filename_output: str, output_bytes: bytes) -> None:
    """
    Write 'mp_sys_version.json' as specified in the file doc string.
    """

    def get_regex(pattern: "re.Pattern[bytes]") -> str:  # re.Pattern is python 3.7+
        match = pattern.search(output_bytes)
        if match is None:
            raise ValueError(f"Could not find '{pattern.pattern!r}' in: {filename_output}")
        text = match.group(1)
        # Example 'mcu_name': "LOLIN_C3_MINI" " with " "ESP32-C3FH4"
        text = eval(text)  # pylint: disable=eval-used
        # Example 'mcu_name': LOLIN_C3_MINI with ESP32-C3FH4
        assert isinstance(text, str)
        return text

    directory_full = os.path.dirname(os.path.realpath(filename_output))
    if "/ports/" not in directory_full:
        # This if will skip for build in the mpy-cross folder!
        return

    for not_supported_port in ("bare-arm", "minimal", "powerpc", "zephyr"):
        if f"/ports/{not_supported_port}/" in directory_full:
            # This port does not support 'mp_sys_version.json'
            return

    # static const mp_obj_str_t mp_sys_version_obj = {{&mp_type_str}, 0, sizeof("3.4.0; " "MicroPython " "v1.24.1" " on " "2024-12-12") - 1, (const byte *)"3.4.0; " "MicroPython " "v1.24.1" " on " "2024-12-12"};
    # static const mp_obj_str_t mp_sys_version_obj = {{&mp_type_str}, 0, sizeof("3.4.0; " "MicroPython " "v1.25.0-preview.147.g136058496" " on " "2024-12-22") - 1, (const byte *)"3.4.0; " "MicroPython " "v1.25.0-preview.147.g136058496" " on " "2024-12-22"};
    _regex_version = re.compile(rb"mp_sys_version_obj.*?sizeof\((\".+?\")\)")

    # static const mp_obj_str_t mp_sys_implementation_machine_obj = {{&mp_type_str}, 0, sizeof("Raspberry Pi Pico2" " with " "RP2350-RISCV") - 1, (const byte *)"Raspberry Pi Pico2" " with " "RP2350-RISCV"};
    # static const mp_obj_str_t mp_sys_implementation_machine_obj = {{&mp_type_str}, 0, sizeof("ESP module (512K)" " with " "ESP8266") - 1, (const byte *)"ESP module (512K)" " with " "ESP8266"};
    _regex_machine = re.compile(rb"mp_sys_implementation_machine_obj.*?sizeof\((\".+?\")\)")

    # static const mp_obj_str_t mp_sys_implementation__build_obj = {{&mp_type_str}, 0, sizeof("PYBV11") - 1, (const byte *)"PYBV11"};
    _regex_build = re.compile(rb"mp_sys_implementation__build_obj.*?sizeof\((\".+?\")\)")

    version_dict = {
        "sys_implementation_build": get_regex(_regex_build),
        "sys_implementation_machine": get_regex(_regex_machine),
        "sys_version": get_regex(_regex_version),
    }

    json_filename = os.path.join(directory_full, "mp_sys_version.json")
    with open(json_filename, "w") as fp:
        json.dump(obj=version_dict, fp=fp, indent=4, sort_keys=True)


def preprocess():
    if any(src in args.dependencies for src in args.changed_sources):
        sources = args.sources
    elif any(args.changed_sources):
        sources = args.changed_sources
    else:
        sources = args.sources
    csources = []
    cxxsources = []
    for source in sources:
        if is_cxx_source(source):
            cxxsources.append(source)
        elif is_c_source(source):
            csources.append(source)
    try:
        os.makedirs(os.path.dirname(args.output[0]))
    except OSError:
        pass

    # These regex's are used to filter the preprocessed data, keeping only those lines
    # that are subsequently needed by the `process_file` step.  The regexs are kept
    # short so they are as efficient as possible. (The stm32 port needs symbols of the
    # form `micropy_hw_xxx` so they are also kept.)
    re_line_file = re.compile(rb"^#(?:line)?\s+\d+\s\"")
    re_mp_info = re.compile(rb"MP_COMP|MP_QSTR|MP_REGI|micropy_hw|mp_sys_")

    def pp(flags):
        def run(files):
            try:
                filtered_lines = []
                with subprocess.Popen(args.pp + flags + files, stdout=subprocess.PIPE) as proc:
                    recent_file = None
                    for line in proc.stdout:
                        if line.isspace():
                            pass
                        elif re_line_file.match(line):
                            recent_file = line
                        elif re_mp_info.search(line):
                            if recent_file:
                                filtered_lines.append(recent_file)
                                recent_file = None
                            filtered_lines.append(line)
                return b"".join(filtered_lines)
            except subprocess.CalledProcessError as er:
                raise PreprocessorError(str(er))

        return run

    try:
        cpus = multiprocessing.cpu_count()
    except NotImplementedError:
        cpus = 1
    p = multiprocessing.dummy.Pool(cpus)
    output_buffer = io.BytesIO()
    for flags, sources in (
        (args.cflags, csources),
        (args.cxxflags, cxxsources),
    ):
        batch_size = (len(sources) + cpus - 1) // cpus
        chunks = [sources[i : i + batch_size] for i in range(0, len(sources), batch_size or 1)]
        for output in p.imap(pp(flags), chunks):
            output_buffer.write(output)

    output_bytes = output_buffer.getvalue()
    with open(args.output[0], "wb") as f:
        f.write(output_bytes)

    write_version_json(filename_output=args.output[0], output_bytes=output_bytes)


def write_out(fname, output):
    if output:
        for m, r in [("/", "__"), ("\\", "__"), (":", "@"), ("..", "@@")]:
            fname = fname.replace(m, r)
        with open(args.output_dir + "/" + fname + "." + args.mode, "w") as f:
            f.write("\n".join(output) + "\n")


def process_file(f):
    # match gcc-like output (# n "file") and msvc-like output (#line n "file")
    re_line = re.compile(r"^#(?:line)?\s+\d+\s\"([^\"]+)\"")
    if args.mode == _MODE_QSTR:
        re_match = re.compile(r"MP_QSTR_[_a-zA-Z0-9]+")
    elif args.mode == _MODE_COMPRESS:
        re_match = re.compile(r'MP_COMPRESSED_ROM_TEXT\("([^"]*)"\)')
    elif args.mode == _MODE_MODULE:
        re_match = re.compile(
            r"(?:MP_REGISTER_MODULE|MP_REGISTER_EXTENSIBLE_MODULE|MP_REGISTER_MODULE_DELEGATION)\(.*?,\s*.*?\);"
        )
    elif args.mode == _MODE_ROOT_POINTER:
        re_match = re.compile(r"MP_REGISTER_ROOT_POINTER\(.*?\);")
    output = []
    last_fname = None
    for line in f:
        if line.isspace():
            continue
        m = re_line.match(line)
        if m:
            fname = m.group(1)
            if not is_c_source(fname) and not is_cxx_source(fname):
                continue
            if fname != last_fname:
                write_out(last_fname, output)
                output = []
                last_fname = fname
            continue
        for match in re_match.findall(line):
            if args.mode == _MODE_QSTR:
                name = match.replace("MP_QSTR_", "")
                output.append("Q(" + name + ")")
            elif args.mode in (_MODE_COMPRESS, _MODE_MODULE, _MODE_ROOT_POINTER):
                output.append(match)

    if last_fname:
        write_out(last_fname, output)
    return ""


def cat_together():
    import glob
    import hashlib

    hasher = hashlib.md5()
    all_lines = []
    for fname in glob.glob(args.output_dir + "/*." + args.mode):
        with open(fname, "rb") as f:
            lines = f.readlines()
            all_lines += lines
    all_lines.sort()
    all_lines = b"\n".join(all_lines)
    hasher.update(all_lines)
    new_hash = hasher.hexdigest()
    # print(new_hash)
    old_hash = None
    try:
        with open(args.output_file + ".hash") as f:
            old_hash = f.read()
    except IOError:
        pass
    mode_full = "QSTR"
    if args.mode == _MODE_COMPRESS:
        mode_full = "Compressed data"
    elif args.mode == _MODE_MODULE:
        mode_full = "Module registrations"
    elif args.mode == _MODE_ROOT_POINTER:
        mode_full = "Root pointer registrations"
    if old_hash != new_hash or not os.path.exists(args.output_file):
        print(mode_full, "updated")

        with open(args.output_file, "wb") as outf:
            outf.write(all_lines)
        with open(args.output_file + ".hash", "w") as f:
            f.write(new_hash)
    else:
        print(mode_full, "not updated")


if __name__ == "__main__":
    if len(sys.argv) < 6:
        print("usage: %s command mode input_filename output_dir output_file" % sys.argv[0])
        sys.exit(2)

    class Args:
        pass

    args = Args()
    args.command = sys.argv[1]

    if args.command == "pp":
        named_args = {
            s: []
            for s in [
                "pp",
                "output",
                "cflags",
                "cxxflags",
                "sources",
                "changed_sources",
                "dependencies",
            ]
        }

        for arg in sys.argv[1:]:
            if arg in named_args:
                current_tok = arg
            else:
                named_args[current_tok].append(arg)

        if not named_args["pp"] or len(named_args["output"]) != 1:
            print("usage: %s %s ..." % (sys.argv[0], " ... ".join(named_args)))
            sys.exit(2)

        for k, v in named_args.items():
            setattr(args, k, v)

        try:
            preprocess()
        except PreprocessorError as er:
            print(er)
            sys.exit(1)

        sys.exit(0)

    args.mode = sys.argv[2]
    args.input_filename = sys.argv[3]  # Unused for command=cat
    args.output_dir = sys.argv[4]
    args.output_file = None if len(sys.argv) == 5 else sys.argv[5]  # Unused for command=split

    if args.mode not in (_MODE_QSTR, _MODE_COMPRESS, _MODE_MODULE, _MODE_ROOT_POINTER):
        print("error: mode %s unrecognised" % sys.argv[2])
        sys.exit(2)

    try:
        os.makedirs(args.output_dir)
    except OSError:
        pass

    if args.command == "split":
        with io.open(args.input_filename, encoding="utf-8") as infile:
            process_file(infile)

    if args.command == "cat":
        cat_together()
