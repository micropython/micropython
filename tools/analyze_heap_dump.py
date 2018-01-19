# This script renders a graph of the MicroPython heap at the given point it was dumped.
# It takes three files, the binary dump of ram, the binary for CircuitPython and the linker map file.

# To dump ram do this in GDB: dump binary memory ram.bin &_srelocate &_estack

import binascii
import struct
import sys
import pygraphviz as pgv
import io
import html

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

SKIP_SYMBOLS = [".debug_ranges", ".debug_frame", ".debug_loc", ".comment", ".debug_str", ".debug_line", ".debug_abbrev", ".debug_info", "COMMON"]

ownership_graph = pgv.AGraph(directed=True)

with open(sys.argv[1], "rb") as f:
    ram = f.read()

with open(sys.argv[2], "rb") as f:
    rom = f.read()

symbols = {} # name -> address, size
symbol_lookup = {} # address -> name
manual_symbol_map = {} # autoname -> name

def add_symbol(name, address=None, size=None):
    global symbols
    if address:
        address = int(address, 0)
    if size:
        size = int(size, 0)
    if name in symbols:
        if address and symbols[name][0] and symbols[name][0] != address:
            print("Conflicting symbol: {}".format(name))
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

with open(sys.argv[3], "r") as f:
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
            if len(parts) == 2 and parts[0].startswith("0x") and not parts[1].startswith("0x"):
                add_symbol(parts[1], parts[0])
            if len(parts) == 4 and parts[0] not in SKIP_SYMBOLS and parts[1].startswith("0x") and parts[2].startswith("0x"):
                name, address, size, source = parts
                if name.startswith((".text", ".rodata", ".bss")) and name.count(".") > 1:
                    name = name.split(".")[-1]
                    add_symbol(name, address, size)
            # Linker symbols
            if len(parts) >= 4 and parts[0].startswith("0x") and parts[2] == "=" and parts[1] != ".":
                add_symbol(parts[1], parts[0])

rom_start = symbols["_sfixed"][0]
ram_start = symbols["_srelocate"][0]

def load(address, size=4):
    if size is None:
        raise ValueError("You must provide a size")
    if address > ram_start:
        ram_address = address - ram_start
        if (ram_address + size) > len(ram):
            raise ValueError("Unable to read 0x{:08x} from ram.".format(address))
        return ram[ram_address:ram_address+size]
    elif address < len(rom):
        if (address + size) > len(rom):
            raise ValueError("Unable to read 0x{:08x} from rom.".format(address))
        return rom[address:address+size]

def load_pointer(address):
    return struct.unpack("<I", load(address))[0]

heap_start, heap_size = symbols["heap"]
heap = load(heap_start, heap_size)
total_byte_len = len(heap)

# These change every run so we load them from the symbol table
mp_state_ctx = symbols["mp_state_ctx"][0]
manual_symbol_map["mp_state_ctx+24"] = "mp_state_ctx.vm.last_pool"
last_pool = load_pointer(mp_state_ctx + 24) # (gdb) p &mp_state_ctx.vm.last_pool
manual_symbol_map["mp_state_ctx+92"] = "mp_state_ctx.vm.dict_main.map.table"
dict_main_table = load_pointer(mp_state_ctx + 92) # (gdb) p &mp_state_ctx.vm.dict_main.map.table
manual_symbol_map["mp_state_ctx+72"] = "mp_state_ctx.vm.mp_loaded_modules_dict.map.table"
imports_table = load_pointer(mp_state_ctx + 72) # (gdb) p &mp_state_ctx.vm.mp_loaded_modules_dict.map.table

manual_symbol_map["mp_state_ctx+108"] = "mp_state_ctx.vm.mp_sys_path_obj.items"
manual_symbol_map["mp_state_ctx+124"] = "mp_state_ctx.vm.mp_sys_argv_obj.items"

for i in range(READLINE_HIST_SIZE):
    manual_symbol_map["mp_state_ctx+{}".format(128 + i * 4)] = "mp_state_ctx.vm.readline_hist[{}]".format(i)

tuple_type = symbols["mp_type_tuple"][0]
type_type = symbols["mp_type_type"][0]
map_type = symbols["mp_type_map"][0]
dict_type = symbols["mp_type_dict"][0]
property_type = symbols["mp_type_property"][0]
str_type = symbols["mp_type_str"][0]
function_types = [symbols["mp_type_fun_" + x][0] for x in ["bc", "builtin_0", "builtin_1", "builtin_2", "builtin_3", "builtin_var"]]
bytearray_type = symbols["mp_type_bytearray"][0]

dynamic_type = 0x40000000 # placeholder, doesn't match any memory

type_colors = {
    dict_type: "red",
    property_type: "yellow",
    map_type: "blue",
    type_type: "orange",
    tuple_type: "skyblue",
    str_type: "pink",
    bytearray_type: "purple"
    }

pool_shift = heap_start % BYTES_PER_BLOCK

print("Total heap length:", total_byte_len)
atb_length = total_byte_len * BITS_PER_BYTE // (BITS_PER_BYTE + BITS_PER_BYTE * BLOCKS_PER_ATB // BLOCKS_PER_FTB + BITS_PER_BYTE * BLOCKS_PER_ATB * BYTES_PER_BLOCK)

print("ATB length:", atb_length)
pool_length = atb_length * BLOCKS_PER_ATB * BYTES_PER_BLOCK
print("Total allocatable:", pool_length)

gc_finaliser_table_byte_len = (atb_length * BLOCKS_PER_ATB + BLOCKS_PER_FTB - 1) // BLOCKS_PER_FTB
print("FTB length:", gc_finaliser_table_byte_len)

pool_start = heap_start + total_byte_len - pool_length - pool_shift
pool = heap[-pool_length-pool_shift:]

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
            print("{} bytes free".format(current_free * BYTES_PER_BLOCK))
            current_free = 0
        if block_state != AT_TAIL and current_allocation > 0:
            allocation_length = current_allocation * BYTES_PER_BLOCK
            end = (i * BLOCKS_PER_ATB + j) * BYTES_PER_BLOCK
            start = end - allocation_length
            address = pool_start + start
            data = pool[start:end]
            print("0x{:x} {} bytes allocated".format(address, allocation_length))

            rows = ""
            for k in range(current_allocation - 1):
                rows += "<tr>"
                for l in range(4):
                    rows += "<td port=\"{}\" height=\"18\" width=\"20\"></td>".format(4 * (k + 1) + l)
                rows += "</tr>"
            table = "<<table bgcolor=\"gray\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\"><tr><td colspan=\"4\" port=\"0\" height=\"18\" width=\"80\">0x{:08x}</td></tr>{}</table>>".format(address, rows)

            ownership_graph.add_node(address, label=table, style="invisible", shape="plaintext")
            potential_type = None
            node = ownership_graph.get_node(address)
            block_data[address] = data
            for k in range(len(data) // 4):
                word = struct.unpack_from("<I", data, offset=(k * 4))[0]
                if word < 0x00040000 and k == 0 or address in qstr_pools:
                    potential_type = word
                    bgcolor = "gray"
                    if address in qstr_pools:
                        bgcolor = "tomato"
                    elif potential_type in function_types:
                        bgcolor = "green"
                    elif potential_type in type_colors:
                        bgcolor = type_colors[potential_type]
                    else:
                        pass
                        #print("unknown type", hex(potential_type))
                    node.attr["label"] = "<" + node.attr["label"].replace("\"gray\"", "\"" + bgcolor + "\"") + ">"

                if potential_type == str_type and k == 3:
                    string_blocks.append(word)


                if potential_type == dict_type:
                    if k == 3:
                        map_element_blocks.append(word)

                if ram_start < word < (ram_start + len(ram)) and word % 16 == 0:
                    port = k
                    if k < 4:
                        port = 0
                    ownership_graph.add_edge(address, word, tailport=str(port)+":_")
                    #print("  0x{:08x}".format(word))
                    if address in qstr_pools:
                        if k > 0:
                            qstr_chunks.append(word)
                    if k == 0:
                        potential_type = dynamic_type


                if potential_type == dynamic_type:
                    if k == 0:
                        node.attr["fillcolor"] = "plum"
                    if k == 3 and 0x20000000 < word < 0x20040000:
                        map_element_blocks.append(word)

                if potential_type in function_types:
                    if k == 2 and 0x20000000 < word < 0x20040000:
                        bytecode_blocks.append(word)

            current_allocation = 0
        if block_state == AT_FREE:
            current_free += 1
            total_free += 1
        elif block_state == AT_HEAD:
            current_allocation = 1
        elif block_state == AT_TAIL:
            current_allocation += 1
        longest_free = max(longest_free, current_free)
if current_free > 0:
    print("{} bytes free".format(current_free * BYTES_PER_BLOCK))

def is_qstr(obj):
    return obj & 0xff800007 == 0x00000006

def find_qstr(qstr_index):
    pool_ptr = last_pool
    if not is_qstr(qstr_index):
        return "object"
    qstr_index >>= 3
    while pool_ptr != 0:
        #print(hex(pool_ptr))
        if pool_ptr in block_data:
            pool = block_data[pool_ptr]
            prev, total_prev_len, alloc, length = struct.unpack_from("<IIII", pool)
        else:
            rom_offset = pool_ptr - rom_start
            prev, total_prev_len, alloc, length = struct.unpack_from("<IIII", rom[rom_offset:rom_offset+32])
            pool = rom[rom_offset:rom_offset+length*4]
            #print("rom pool")
        #print(hex(prev), total_prev_len, alloc, length)
        #print(qstr_index, total_prev_len)
        if qstr_index >= total_prev_len:
            offset = (qstr_index - total_prev_len) * 4 + 16
            start = struct.unpack_from("<I", pool, offset=offset)[0]
            #print(hex(start))
            if start < heap_start:
                start -= rom_start
                if start > len(rom):
                    return "more than rom: {:x}".format(start + rom_start)
                qstr_hash, qstr_len = struct.unpack("<BB", rom[start:start+2])
                return rom[start+2:start+2+qstr_len].decode("utf-8")
            else:
                if start > heap_start + len(heap):
                    return "out of range: {:x}".format(start)
                local = start - heap_start
                qstr_hash, qstr_len = struct.unpack("<BB", heap[local:local+2])
                return heap[local+2:local+2+qstr_len].decode("utf-8")

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
    try:
        node = ownership_graph.get_node(block)
    except KeyError:
        print("Unable to find memory block for 0x{:08x}. Is there something running?".format(block))
        continue
    #node.attr["fillcolor"] = "gold"
    data = block_data[block]
    #print("0x{:08x}".format(block))
    cells = []
    for i in range(len(data) // 8):
        key, value = struct.unpack_from("<II", data, offset=(i * 8))
        if key == MP_OBJ_NULL or key == MP_OBJ_SENTINEL:
            #print("  <empty slot>")
            cells.append(("", " "))
        else:
            #print("  {}, {}".format(format(key), format(value)))
            cells.append((key, format(key)))
            if value in block_data:
                edge = ownership_graph.get_edge(block, value)
                edge.attr["tailport"] = str(key)
    rows = ""
    for i in range(len(cells) // 2):
        rows += "<tr><td port=\"{}\">{}</td><td port=\"{}\">{}</td></tr>".format(
            cells[2*i][0],
            cells[2*i][1],
            cells[2*i+1][0],
            cells[2*i+1][1])
    node.attr["shape"] = "plaintext"
    node.attr["style"] = "invisible"
    node.attr["label"] = "<<table bgcolor=\"gold\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\"><tr><td colspan=\"2\">0x{:08x}</td></tr>{}</table>>".format(block, rows)

for node, degree in ownership_graph.in_degree_iter():
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
            ownership_graph.add_edge(source, node)

for block in string_blocks:
    node = ownership_graph.get_node(block)
    node.attr["fillcolor"] = "hotpink"
    string = block_data[block].decode('utf-8')
    wrapped = []
    for i in range(0, len(string), 16):
        wrapped.append(string[i:i+16])
    node.attr["label"] = "\n".join(wrapped)
    node.attr["style"] = "filled"
    node.attr["fontname"] = "FiraCode-Medium"
    node.attr["fontpath"] = "/Users/tannewt/Library/Fonts/"
    node.attr["fontsize"] = 8

for block in bytecode_blocks:
    node = ownership_graph.get_node(block)
    node.attr["fillcolor"] = "lightseagreen"
    data = block_data[block]
    prelude = Prelude(io.BufferedReader(io.BytesIO(data)))
    node.attr["shape"] = "plaintext"
    node.attr["style"] = "invisible"
    code_info_size = prelude.code_info_size
    rows = ""
    remaining_bytecode = len(data) - 16
    while code_info_size >= 16:
        rows += "<tr><td colspan=\"16\" bgcolor=\"palegreen\" height=\"18\" width=\"80\"></td></tr>"
        code_info_size -= 16
        remaining_bytecode -= 16
    if code_info_size > 0:
        rows += ("<tr><td colspan=\"{}\" bgcolor=\"palegreen\" height=\"18\" width=\"{}\"></td>"
                 "<td colspan=\"{}\" bgcolor=\"seagreen\" height=\"18\" width=\"{}\"></td></tr>"
                ).format(code_info_size, code_info_size * (80 / 16), (16 - code_info_size), (80 / 16) * (16 - code_info_size))
        remaining_bytecode -= 16
    for i in range(remaining_bytecode // 16):
        rows += "<tr><td colspan=\"16\" bgcolor=\"seagreen\" height=\"18\" width=\"80\"></td></tr>"
    node.attr["label"] = "<<table border=\"1\" cellspacing=\"0\"><tr><td colspan=\"16\" bgcolor=\"lightseagreen\" height=\"18\" width=\"80\">0x{:08x}</td></tr>{}</table>>".format(block, rows)

for block in qstr_chunks:
    if block not in block_data:
        ownership_graph.delete_node(block)
        continue
    data = block_data[block]
    string = ""
    offset = 0
    while offset < len(data) - 1:
        qstr_hash, qstr_len = struct.unpack_from("<BB", data, offset=offset)
        if qstr_hash == 0:
            string += " " * (len(data) - offset)
            offset = len(data)
            continue
        offset += 2 + qstr_len + 1
        string += "  " + data[offset - qstr_len - 1: offset - 1].decode("utf-8")
    #print(string)
    wrapped = []
    for i in range(0, len(string), 16):
        wrapped.append(html.escape(string[i:i+16]))
    node = ownership_graph.get_node(block)
    node.attr["label"] = "<<table border=\"1\" cellspacing=\"0\" bgcolor=\"lightsalmon\" width=\"80\"><tr><td height=\"18\" >0x{:08x}</td></tr><tr><td height=\"{}\" >{}</td></tr></table>>".format(block, 18 * (len(wrapped) - 1), "<br/>".join(wrapped))
    node.attr["fontname"] = "FiraCode-Medium"
    node.attr["fontpath"] = "/Users/tannewt/Library/Fonts/"
    node.attr["fontsize"] = 8

print("Total free space:", BYTES_PER_BLOCK * total_free)
print("Longest free space:", BYTES_PER_BLOCK * longest_free)

with open("heap.dot", "w") as f:
    f.write(ownership_graph.string())

ownership_graph.layout(prog="dot")
ownership_graph.draw("heap.png")
