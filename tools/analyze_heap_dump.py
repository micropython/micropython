# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

# This script renders a graph of the MicroPython heap at the given point it was dumped.
# It takes three files, the binary dump of ram, the binary for CircuitPython and the linker map file.

# To dump ram do this in GDB: dump binary memory ram.bin &_srelocate &_estack

import binascii
import struct
import sys
import pygraphviz as pgv
import io
import html
import os.path
import string

import click

from analyze_mpy import Prelude

BITS_PER_BYTE = 8
BLOCKS_PER_ATB = 4
BLOCKS_PER_FTB = 8
BYTES_PER_BLOCK = 16

AT_FREE = 0
AT_HEAD = 1
AT_TAIL = 2
AT_MARK = 3

MICROPY_QSTR_BYTES_IN_HASH = 1
MICROPY_QSTR_BYTES_IN_LEN = 1

MP_OBJ_NULL = 0
MP_OBJ_SENTINEL = 4

READLINE_HIST_SIZE = 8

SKIP_SYMBOLS = [
    ".debug_ranges",
    ".debug_frame",
    ".debug_loc",
    ".comment",
    ".debug_str",
    ".debug_line",
    ".debug_abbrev",
    ".debug_info",
    "COMMON",
]


@click.command()
@click.argument("ram_filename")
@click.argument("bin_filename")
@click.argument("map_filename")
@click.option(
    "--print_block_contents", default=False, help="Prints the contents of each allocated block"
)
@click.option(
    "--print_unknown_types",
    default=False,
    help="Prints the micropython base type if we don't understand it.",
)
@click.option(
    "--print_block_state", default=False, help="Prints the heap block states (allocated or free)"
)
@click.option(
    "--print_conflicting_symbols", default=False, help="Prints conflicting symbols from the map"
)
@click.option(
    "--print-heap-structure/--no-print-heap-structure", default=False, help="Print heap structure"
)
@click.option("--output_directory", default="heapvis", help="Destination for rendered output")
@click.option(
    "--draw-heap-layout/--no-draw-heap-layout", default=True, help="Draw the heap layout"
)
@click.option(
    "--draw-heap-ownership/--no-draw-heap-ownership",
    default=False,
    help="Draw the ownership graph of blocks on the heap",
)
@click.option("--analyze-snapshots", default="last", type=click.Choice(["all", "last"]))
def do_all_the_things(
    ram_filename,
    bin_filename,
    map_filename,
    print_block_contents,
    print_unknown_types,
    print_block_state,
    print_conflicting_symbols,
    print_heap_structure,
    output_directory,
    draw_heap_layout,
    draw_heap_ownership,
    analyze_snapshots,
):
    with open(ram_filename, "rb") as f:
        ram_dump = f.read()

    with open(bin_filename, "rb") as f:
        rom = f.read()

    symbols = {}  # name -> address, size
    symbol_lookup = {}  # address -> name
    manual_symbol_map = {}  # autoname -> name

    def add_symbol(name, address=None, size=None):
        if "lto_priv" in name:
            name = name.split(".")[0]
        if address:
            address = int(address, 0)
        if size:
            size = int(size, 0)
        if name in symbols:
            if address and symbols[name][0] and symbols[name][0] != address:
                if print_conflicting_symbols:
                    print(
                        "Conflicting symbol: {} at addresses 0x{:08x} and 0x{:08x}".format(
                            name, address, symbols[name][0]
                        )
                    )
                return
            if not address:
                address = symbols[name][0]
            if not size:
                size = symbols[name][1]
        symbols[name] = (address, size)
        if address:
            if not size:
                size = 4
            for offset in range(0, size, 4):
                symbol_lookup[address + offset] = "{}+{}".format(name, offset)

    with open(map_filename, "r") as f:
        common_symbols = False
        name = None
        for line in f:
            line = line.strip()
            parts = line.split()
            if line.startswith("Common symbol"):
                common_symbols = True
            if line == "Discarded input sections":
                common_symbols = False
            if common_symbols:
                if len(parts) == 1:
                    name = parts[0]
                elif len(parts) == 2 and name:
                    add_symbol(name, size=parts[0])
                    name = None
                elif len(parts) == 3:
                    add_symbol(parts[0], size=parts[1])
                    name = None
            else:
                if (
                    len(parts) == 1
                    and parts[0].startswith((".text", ".rodata", ".bss"))
                    and parts[0].count(".") > 1
                    and not parts[0].isnumeric()
                    and ".str" not in parts[0]
                ):
                    name = parts[0].split(".")[2]
                if (
                    len(parts) == 3
                    and parts[0].startswith("0x")
                    and parts[1].startswith("0x")
                    and name
                ):
                    add_symbol(name, parts[0], parts[1])
                    name = None
                if len(parts) == 2 and parts[0].startswith("0x") and not parts[1].startswith("0x"):
                    add_symbol(parts[1], parts[0])
                if (
                    len(parts) == 4
                    and parts[0] not in SKIP_SYMBOLS
                    and parts[1].startswith("0x")
                    and parts[2].startswith("0x")
                ):
                    name, address, size, source = parts
                    if name.startswith((".text", ".rodata", ".bss")) and name.count(".") > 1:
                        name = name.split(".")[-1]
                        add_symbol(name, address, size)
                    name = None
                # Linker symbols
                if (
                    len(parts) >= 4
                    and parts[0].startswith("0x")
                    and parts[2] == "="
                    and parts[1] != "."
                ):
                    add_symbol(parts[1], parts[0])

    rom_start = symbols["_sfixed"][0]
    ram_start = symbols["_srelocate"][0]
    ram_end = symbols["_estack"][0]
    ram_length = ram_end - ram_start
    # print(ram_length, "ram length")
    # print(len(ram_dump) // ram_length, "snapshots")
    if analyze_snapshots == "all":
        snapshots = range(len(ram_dump) // ram_length - 1, -1, -1)
        # snapshots = range(4576, -1, -1)
    elif analyze_snapshots == "last":
        snapshots = range(len(ram_dump) // ram_length - 1, len(ram_dump) // ram_length - 2, -1)
    for snapshot_num in snapshots:
        ram = ram_dump[ram_length * snapshot_num : ram_length * (snapshot_num + 1)]

        ownership_graph = pgv.AGraph(directed=True)

        def load(address, size=4):
            if size is None:
                raise ValueError("You must provide a size")
            if address > ram_start:
                ram_address = address - ram_start
                if (ram_address + size) > len(ram):
                    raise ValueError("Unable to read 0x{:08x} from ram.".format(address))
                return ram[ram_address : ram_address + size]
            elif address < len(rom):
                if (address + size) > len(rom):
                    raise ValueError("Unable to read 0x{:08x} from rom.".format(address))
                return rom[address : address + size]

        def load_pointer(address):
            return struct.unpack("<I", load(address))[0]

        if "heap" in symbols:
            heap_start, heap_size = symbols["heap"]
        else:
            print("no static heap")
            allocations_start, allocations_size = symbols["allocations"]
            allocations = load(allocations_start, allocations_size)
            first_zero = True
            potential_heap = None
            # The heap is the last left hand allocated section that should span all the way to the
            # right side list.
            for address, size in struct.iter_unpack("<II", allocations):
                print(address, size)
                if address == 0 and first_zero:
                    first_zero = False
                if first_zero:
                    potential_heap = (address, size)

                if not first_zero and address != 0:
                    if address != potential_heap[0] + potential_heap[1]:
                        print("no active heap")
                        return
                    else:
                        heap_start, heap_size = potential_heap
                        break
        print("found heap", heap_start, heap_size)
        heap = load(heap_start, heap_size)
        total_byte_len = len(heap)

        # These change every run so we load them from the symbol table
        mp_state_ctx = symbols["mp_state_ctx"][0]
        manual_symbol_map["mp_state_ctx+20"] = "mp_state_ctx.vm.last_pool"
        last_pool = load_pointer(mp_state_ctx + 20)  # (gdb) p &mp_state_ctx.vm.last_pool
        manual_symbol_map["mp_state_ctx+108"] = "mp_state_ctx.vm.dict_main.map.table"
        dict_main_table = load_pointer(
            mp_state_ctx + 108
        )  # (gdb) p &mp_state_ctx.vm.dict_main.map.table
        manual_symbol_map["mp_state_ctx+84"] = "mp_state_ctx.vm.mp_loaded_modules_dict.map.table"
        imports_table = load_pointer(
            mp_state_ctx + 84
        )  # (gdb) p &mp_state_ctx.vm.mp_loaded_modules_dict.map.table

        manual_symbol_map["mp_state_ctx+124"] = "mp_state_ctx.vm.mp_sys_path_obj.items"
        manual_symbol_map["mp_state_ctx+140"] = "mp_state_ctx.vm.mp_sys_argv_obj.items"
        manual_symbol_map["mp_state_ctx+96"] = "mp_state_ctx.vm.dict_main"
        manual_symbol_map["0x200015e0"] = "mp_state_ctx.vm.dict_main"

        for i in range(READLINE_HIST_SIZE):
            manual_symbol_map[
                "mp_state_ctx+{}".format(148 + i * 4)
            ] = "mp_state_ctx.vm.readline_hist[{}]".format(i)

        tuple_type = symbols["mp_type_tuple"][0]
        type_type = symbols["mp_type_type"][0]
        map_type = symbols["mp_type_map"][0]
        dict_type = symbols["mp_type_dict"][0]
        property_type = symbols["mp_type_property"][0]
        str_type = symbols["mp_type_str"][0]
        function_types = [
            symbols["mp_type_fun_" + x][0]
            for x in ["bc", "builtin_0", "builtin_1", "builtin_2", "builtin_3", "builtin_var"]
        ]
        bytearray_type = symbols["mp_type_bytearray"][0]

        dynamic_type = 0x40000000  # placeholder, doesn't match any memory

        long_lived_start = load_pointer(
            mp_state_ctx + 272
        )  # (gdb) p &mp_state_ctx.mem.gc_lowest_long_lived_ptr

        type_colors = {
            dict_type: "red",
            property_type: "yellow",
            map_type: "blue",
            type_type: "orange",
            tuple_type: "skyblue",
            str_type: "pink",
            bytearray_type: "purple",
        }

        pool_shift = heap_start % BYTES_PER_BLOCK
        atb_length = (
            total_byte_len
            * BITS_PER_BYTE
            // (
                BITS_PER_BYTE
                + BITS_PER_BYTE * BLOCKS_PER_ATB // BLOCKS_PER_FTB
                + BITS_PER_BYTE * BLOCKS_PER_ATB * BYTES_PER_BLOCK
            )
        )
        pool_length = atb_length * BLOCKS_PER_ATB * BYTES_PER_BLOCK
        gc_finaliser_table_byte_len = (
            atb_length * BLOCKS_PER_ATB + BLOCKS_PER_FTB - 1
        ) // BLOCKS_PER_FTB

        if print_heap_structure:
            print("mp_state_ctx at 0x{:08x} and length {}".format(*symbols["mp_state_ctx"]))
            print("Total heap length:", total_byte_len)
            print("ATB length:", atb_length)
            print("Total allocatable:", pool_length)
            print("FTB length:", gc_finaliser_table_byte_len)

        pool_start = heap_start + total_byte_len - pool_length - pool_shift
        pool = heap[-pool_length - pool_shift :]

        total_height = 128 * 18
        total_width = (pool_length // (128 * 16)) * 85

        map_element_blocks = [dict_main_table, imports_table]
        string_blocks = []
        bytecode_blocks = []
        qstr_pools = []
        qstr_chunks = []
        block_data = {}

        # Find all the qtr pool addresses.
        prev_pool = last_pool
        while prev_pool > ram_start:
            qstr_pools.append(prev_pool)
            prev_pool = load_pointer(prev_pool)

        def save_allocated_block(end, current_allocation):
            allocation_length = current_allocation * BYTES_PER_BLOCK
            start = end - allocation_length
            address = pool_start + start
            data = pool[start:end]
            if print_block_state:
                print("0x{:x} {} bytes allocated".format(address, allocation_length))
            if print_block_contents:
                print(data)

            rows = ""
            for k in range(current_allocation - 1):
                rows += "<tr>"
                for l in range(4):
                    rows += '<td port="{}" height="18" width="20"></td>'.format(4 * (k + 1) + l)
                rows += "</tr>"
            table = '<<table bgcolor="gray" border="1" cellpadding="0" cellspacing="0"><tr><td colspan="4" port="0" height="18" width="80">0x{:08x}</td></tr>{}</table>>'.format(
                address, rows
            )

            ownership_graph.add_node(address, label=table, style="invisible", shape="plaintext")
            print("add  0x{:08x}".format(address))
            potential_type = None
            node = ownership_graph.get_node(address)
            node.attr["height"] = 0.25 * current_allocation
            if address >= long_lived_start:
                node.attr["fontcolor"] = "hotpink"
            else:
                node.attr["fontcolor"] = "black"
            block_data[address] = data
            for k in range(len(data) // 4):
                word = struct.unpack_from("<I", data, offset=(k * 4))[0]
                if word < len(rom) and k == 0 or address in qstr_pools:
                    potential_type = word
                    bgcolor = "gray"
                    if address in qstr_pools:
                        # print(address, len(data))
                        bgcolor = "tomato"
                    elif potential_type in function_types:
                        bgcolor = "green"
                    elif potential_type in type_colors:
                        bgcolor = type_colors[potential_type]
                    elif print_unknown_types:
                        print("unknown type", hex(potential_type))

                    node.attr["label"] = (
                        "<" + node.attr["label"].replace('"gray"', '"' + bgcolor + '"') + ">"
                    )

                if potential_type == str_type and k == 3:
                    string_blocks.append(word)

                if potential_type == dict_type:
                    if k == 3:
                        map_element_blocks.append(word)

                if ram_start < word < (ram_start + len(ram)) and word % 16 == 0:
                    port = k
                    if k < 4:
                        port = 0
                    ownership_graph.add_edge(address, word, tailport=str(port) + ":_")
                    print("  0x{:08x}".format(word))
                    if address in qstr_pools:
                        if k > 0:
                            qstr_chunks.append(word)
                    if k == 0:
                        potential_type = dynamic_type

                if potential_type == dynamic_type:
                    if k == 0:
                        node.attr["fillcolor"] = "plum"
                    if k == 3 and ram_start < word < ram_end:
                        map_element_blocks.append(word)

                if potential_type in function_types:
                    if k == 2 and ram_start < word < ram_end:
                        bytecode_blocks.append(word)

        longest_free = 0
        current_free = 0
        current_allocation = 0
        total_free = 0
        for i in range(atb_length):
            # Each atb byte is four blocks worth of info
            atb = heap[i]
            for j in range(4):
                block_state = (atb >> (j * 2)) & 0x3
                if block_state != AT_FREE and current_free > 0:
                    if print_block_state:
                        print("{} bytes free".format(current_free * BYTES_PER_BLOCK))
                    current_free = 0
                if block_state != AT_TAIL and current_allocation > 0:
                    save_allocated_block(
                        (i * BLOCKS_PER_ATB + j) * BYTES_PER_BLOCK, current_allocation
                    )
                    current_allocation = 0
                if block_state == AT_FREE:
                    current_free += 1
                    total_free += 1
                elif block_state == AT_HEAD or block_state == AT_MARK:
                    current_allocation = 1
                elif block_state == AT_TAIL and current_allocation > 0:
                    # In gc_free the logging happens before the tail is freed. So checking
                    # current_allocation > 0 ensures we only extend an allocation thats started.
                    current_allocation += 1
                longest_free = max(longest_free, current_free)
        # if current_free > 0:
        #    print("{} bytes free".format(current_free * BYTES_PER_BLOCK))
        if current_allocation > 0:
            save_allocated_block(pool_length, current_allocation)

        def is_qstr(obj):
            return obj & 0xFF800007 == 0x00000006

        def find_qstr(qstr_index):
            pool_ptr = last_pool
            if not is_qstr(qstr_index):
                return "object"

            pool = block_data[pool_ptr]
            prev, total_prev_len, alloc, length = struct.unpack_from("<IIII", pool)
            qstr_index >>= 3
            if qstr_index > total_prev_len + alloc:
                return "invalid"
            while pool_ptr != 0:
                if pool_ptr > ram_start:
                    if pool_ptr in block_data:
                        pool = block_data[pool_ptr]
                        prev, total_prev_len, alloc, length = struct.unpack_from("<IIII", pool)
                    else:
                        print("missing qstr pool: {:08x}".format(pool_ptr))
                        return "missing"
                else:
                    rom_offset = pool_ptr - rom_start
                    prev, total_prev_len, alloc, length = struct.unpack_from(
                        "<IIII", rom[rom_offset : rom_offset + 32]
                    )
                    pool = rom[rom_offset : rom_offset + 32 + length * 4]

                if qstr_index >= total_prev_len:
                    offset = (qstr_index - total_prev_len) * 4 + 16
                    start = struct.unpack_from("<I", pool, offset=offset)[0]
                    if start < heap_start:
                        start -= rom_start
                        if start > len(rom):
                            return "more than rom: {:x}".format(start + rom_start)
                        qstr_hash, qstr_len = struct.unpack("<BB", rom[start : start + 2])
                        return rom[start + 2 : start + 2 + qstr_len].decode("utf-8")
                    else:
                        if start > heap_start + len(heap):
                            return "out of range: {:x}".format(start)
                        local = start - heap_start
                        qstr_hash, qstr_len = struct.unpack("<BB", heap[local : local + 2])
                        return heap[local + 2 : local + 2 + qstr_len].decode("utf-8")

                pool_ptr = prev
            return "unknown"

        def format(obj):
            if obj & 1 != 0:
                return obj >> 1
            if is_qstr(obj):
                return find_qstr(obj)
            else:
                return "0x{:08x}".format(obj)

        for block in sorted(map_element_blocks):
            if block == 0:
                continue
            try:
                node = ownership_graph.get_node(block)
            except KeyError:
                print(
                    "Unable to find memory block for 0x{:08x}. Is there something running?".format(
                        block
                    )
                )
                continue
            if block not in block_data:
                continue
            data = block_data[block]
            cells = []
            for i in range(len(data) // 8):
                key, value = struct.unpack_from("<II", data, offset=(i * 8))
                if key == MP_OBJ_NULL or key == MP_OBJ_SENTINEL:
                    cells.append(("", " "))
                else:
                    cells.append((key, format(key)))
                    if value in block_data:
                        edge = ownership_graph.get_edge(block, value)
                        edge.attr["tailport"] = str(key)
            rows = ""
            for i in range(len(cells) // 2):
                rows += '<tr><td port="{}">{}</td><td port="{}">{}</td></tr>'.format(
                    cells[2 * i][0], cells[2 * i][1], cells[2 * i + 1][0], cells[2 * i + 1][1]
                )
            node.attr["shape"] = "plaintext"
            node.attr["style"] = "invisible"
            node.attr[
                "label"
            ] = '<<table bgcolor="gold" border="1" cellpadding="0" cellspacing="0"><tr><td colspan="2">0x{:08x}</td></tr>{}</table>>'.format(
                block, rows
            )

        for node, degree in ownership_graph.in_degree_iter():
            print(node, degree)
            if degree == 0:
                address_bytes = struct.pack("<I", int(node))
                location = -1
                for _ in range(ram.count(address_bytes)):
                    location = ram.find(address_bytes, location + 1)
                    pointer_location = ram_start + location
                    source = "0x{:08x}".format(pointer_location)
                    if pointer_location in symbol_lookup:
                        source = symbol_lookup[pointer_location]
                    if source in manual_symbol_map:
                        source = manual_symbol_map[source]
                    if "readline_hist" in source:
                        string_blocks.append(int(node))
                    if pointer_location > heap_start + heap_size:
                        source = "stack " + source
                    ownership_graph.add_edge(source, node)

        for block in string_blocks:
            if block == 0:
                continue
            node = ownership_graph.get_node(block)
            node.attr["fillcolor"] = "hotpink"
            if block in block_data:
                raw_string = block_data[block]
            else:
                print("Unable to find memory block for string at 0x{:08x}.".format(block))
                continue
            try:
                raw_string = block_data[block].decode("utf-8")
            except:
                raw_string = str(block_data[block])
            wrapped = []
            for i in range(0, len(raw_string), 16):
                wrapped.append(raw_string[i : i + 16])
            node.attr["label"] = "\n".join(wrapped)
            node.attr["style"] = "filled"
            node.attr["fontname"] = "FiraCode-Medium"
            node.attr["fontpath"] = "/Users/tannewt/Library/Fonts/"
            node.attr["fontsize"] = 8
            node.attr["height"] = len(wrapped) * 0.25

        for block in bytecode_blocks:
            node = ownership_graph.get_node(block)
            node.attr["fillcolor"] = "lightseagreen"
            if block in block_data:
                data = block_data[block]
            else:
                print("Unable to find memory block for bytecode at 0x{:08x}.".format(block))
                continue
            prelude = Prelude(io.BufferedReader(io.BytesIO(data)))
            node.attr["shape"] = "plaintext"
            node.attr["style"] = "invisible"
            code_info_size = prelude.code_info_size
            rows = ""
            remaining_bytecode = len(data) - 16
            while code_info_size >= 16:
                rows += (
                    '<tr><td colspan="16" bgcolor="palegreen" height="18" width="80"></td></tr>'
                )
                code_info_size -= 16
                remaining_bytecode -= 16
            if code_info_size > 0:
                rows += (
                    '<tr><td colspan="{}" bgcolor="palegreen" height="18" width="{}"></td>'
                    '<td colspan="{}" bgcolor="seagreen" height="18" width="{}"></td></tr>'
                ).format(
                    code_info_size,
                    code_info_size * (80 / 16),
                    (16 - code_info_size),
                    (80 / 16) * (16 - code_info_size),
                )
                remaining_bytecode -= 16
            for i in range(remaining_bytecode // 16):
                rows += '<tr><td colspan="16" bgcolor="seagreen" height="18" width="80"></td></tr>'
            node.attr[
                "label"
            ] = '<<table border="1" cellspacing="0"><tr><td colspan="16" bgcolor="lightseagreen" height="18" width="80">0x{:08x}</td></tr>{}</table>>'.format(
                block, rows
            )

        for block in qstr_chunks:
            if block not in block_data:
                ownership_graph.delete_node(block)
                continue
            data = block_data[block]
            qstrs_in_chunk = ""
            offset = 0
            while offset < len(data) - 1:
                qstr_hash, qstr_len = struct.unpack_from("<BB", data, offset=offset)
                if qstr_hash == 0:
                    qstrs_in_chunk += " " * (len(data) - offset)
                    offset = len(data)
                    continue
                offset += 2 + qstr_len + 1
                try:
                    qstrs_in_chunk += "  " + data[offset - qstr_len - 1 : offset - 1].decode(
                        "utf-8"
                    )
                except UnicodeDecodeError:
                    qstrs_in_chunk += "  " + "░" * qstr_len
            printable_qstrs = ""
            for i in range(len(qstrs_in_chunk)):
                c = qstrs_in_chunk[i]
                if c not in string.printable or c in "\v\f":
                    printable_qstrs += "░"
                else:
                    printable_qstrs += qstrs_in_chunk[i]
            wrapped = []
            for i in range(0, len(printable_qstrs), 16):
                wrapped.append(html.escape(printable_qstrs[i : i + 16]))
            node = ownership_graph.get_node(block)
            node.attr[
                "label"
            ] = '<<table border="1" cellspacing="0" bgcolor="lightsalmon" width="80"><tr><td height="18" >0x{:08x}</td></tr><tr><td height="{}" >{}</td></tr></table>>'.format(
                block, 18 * (len(wrapped) - 1), "<br/>".join(wrapped)
            )
            node.attr["fontname"] = "FiraCode-Bold"
            if block >= long_lived_start:
                node.attr["fontcolor"] = "hotpink"
            else:
                node.attr["fontcolor"] = "black"
            node.attr["fontpath"] = "/Users/tannewt/Library/Fonts/"
            node.attr["fontsize"] = 8

        print("Total free space:", BYTES_PER_BLOCK * total_free)
        print("Longest free space:", BYTES_PER_BLOCK * longest_free)

        # First render the graph of objects on the heap.
        if draw_heap_ownership:
            ownership_graph.layout(prog="dot")
            fn = os.path.join(output_directory, "heap_ownership{:04d}.svg".format(snapshot_num))
            print(fn)
            ownership_graph.draw(fn)

        # Clear edge positioning from ownership graph layout.
        if draw_heap_ownership:
            for edge in ownership_graph.iteredges():
                del edge.attr["pos"]
        else:
            for edge in ownership_graph.edges():
                ownership_graph.delete_edge(edge)

        # Second, render the heap layout in memory order.
        for node in ownership_graph.nodes():
            try:
                address = int(node.name)
            except ValueError:
                ownership_graph.remove_node(node.name)
                continue
            block = (address - pool_start) // 16
            x = block // 128
            y = 128 - block % 128
            try:
                height = float(node.attr["height"])
            except:
                height = 0.25
            # print(hex(address), "height", height, y)
            # if address in block_data:
            #    print(hex(address), block, len(block_data[address]), x, y, height)
            node.attr["pos"] = "{},{}".format(x * 80, (y - (height - 0.25) * 2) * 18)  # in inches

        # Reformat block nodes so they are the correct size and do not have keys in them.
        for block in sorted(map_element_blocks):
            try:
                node = ownership_graph.get_node(block)
            except KeyError:
                if block != 0:
                    print(
                        "Unable to find memory block for 0x{:08x}. Is there something running?".format(
                            block
                        )
                    )
                continue
            # node.attr["fillcolor"] = "gold"
            if block not in block_data:
                continue
            data = block_data[block]
            # print("0x{:08x}".format(block))
            cells = []
            for i in range(len(data) // 8):
                key, value = struct.unpack_from("<II", data, offset=(i * 8))
                if key == MP_OBJ_NULL or key == MP_OBJ_SENTINEL:
                    # print("  <empty slot>")
                    cells.append(("", " "))
                else:
                    # print("  {}, {}".format(format(key), format(value)))
                    cells.append((key, ""))
                    # if value in block_data:
                    #     edge = ownership_graph.get_edge(block, value)
                    #     edge.attr["tailport"] = str(key)
            rows = ""
            for i in range(len(cells) // 2):
                rows += '<tr><td port="{}" height="18" width="40">{}</td><td port="{}" height="18" width="40">{}</td></tr>'.format(
                    cells[2 * i][0], cells[2 * i][1], cells[2 * i + 1][0], cells[2 * i + 1][1]
                )
            node.attr[
                "label"
            ] = '<<table bgcolor="gold" border="1" cellpadding="0" cellspacing="0">{}</table>>'.format(
                rows
            )

        ownership_graph.add_node(
            "center",
            pos="{},{}".format(total_width // 2 - 40, total_height // 2),
            shape="plaintext",
            label=" ",
        )
        ownership_graph.graph_attr["viewport"] = "{},{},1,{}".format(
            total_width, total_height, "center"
        )

        ownership_graph.has_layout = True

        if draw_heap_layout:
            fn = os.path.join(output_directory, "heap_layout{:04d}.png".format(snapshot_num))
            print(fn)
            # ownership_graph.write(fn+".dot")
            ownership_graph.draw(fn)


if __name__ == "__main__":
    do_all_the_things()
