"""
Process script processes a C source and extracts all preprocessor
directives including along with qstrings. Each qstring is transformed
into a qstr definition of the form 'Q(...)'

This script works with Python 2.6, 2.7, 3.3 and 3.4.
"""

import sys
import re
import argparse
import os

# Blacklist of qstrings that are specially handled in further
# processing and should be ignored
QSTRING_BLACK_LIST = {'NULL', 'number_of', }


def debug(message):
    #sys.stderr.write("%s\n" % message)
    pass


class Region(object):
    def __init__(self):
        self.qstrs = []
        self.child_nodes = []

    def add_qstr(self, qstr):
        self.qstrs.append(qstr)

    def add_child(self, child):
        self.child_nodes.append(child)

    def is_empty(self):
        return len(self.qstrs) == 0 and all(child.is_empty() for child in self.child_nodes)

    def validate(self):
        for child in self.child_nodes:
            child.validate()

    def generate_output(self, output):
        for qstr in self.qstrs:
            qstr.generate_output(output)
        for child in self.child_nodes:
            child.generate_output(output)


class IfNode(object):
    def __init__(self, intro_line):
        self.src_lines = []
        self.regions = []
        self.final_region = False
        self.finished = False
        self.add_region(intro_line)

    @property
    def active_region(self):
        return self.regions[-1]

    def is_empty(self):
        return all(region.is_empty() for region in self.regions)

    def add_region(self, src_line, final=False):
        assert not self.finished
        assert not self.final_region, '#else found after #else'

        self.final_region = final
        self.src_lines.append(src_line)
        self.regions.append(Region())

    def finish(self, end_line):
        assert not self.finished
        assert len(self.regions) > 0
        self.finished = True
        self.src_lines.append(end_line)

    def validate(self):
        if not self.finished:
            raise Exception('if/ifdef not finished: %s' % self.src_lines[0])
        for region in self.regions:
            region.validate()

    def generate_output(self, output):
        if self.is_empty():
            return
        for idx, region in enumerate(self.regions):
            output.append(self.src_lines[idx])
            output.indent()
            region.generate_output(output)
            output.dedent()
        output.append(self.src_lines[-1])


class OtherDirectiveNode(object):
    def __init__(self, src_line):
        self.src_line = src_line

    def generate_output(self, output):
        output.append(self.src_line)

    def validate(self):
        pass

    def is_empty(self):
        return False


class FileNode(object):
    def __init__(self):
        self.active_region = Region()

    @staticmethod
    def finish(src_line):
        raise Exception('#endif found without #if/#ifdef')


class NodeStack(object):
    def __init__(self):
        self.top_node = None
        self.node_stack = []
        self._push_node(FileNode())

    def _top_region(self):
        return self.top_node.active_region

    def _push_node(self, node):
        self.top_node = node
        self.node_stack.append(node)

    def _pop_node(self):
        assert len(self.node_stack) > 1
        self.node_stack.pop()
        self.top_node = self.node_stack[-1]

    def push_if(self, src_line):
        node = IfNode(src_line)
        self._top_region().add_child(node)
        self._push_node(node)

    def push_elif(self, src_line):
        self.top_node.add_region(src_line)

    def push_else(self, src_line):
        self.top_node.add_region(src_line, final=True)

    def push_endif(self, src_line):
        self.top_node.finish(src_line)
        self._pop_node()

    def push_other_directive(self, src_line):
        node = OtherDirectiveNode(src_line)
        self._top_region().add_child(node)

    def push_qstring(self, qstring):
        self._top_region().add_qstr(qstring)

    def get_result(self):
        assert len(self.node_stack) == 1
        assert isinstance(self.top_node, FileNode)
        return self.top_node.active_region


class QString(object):
    def __init__(self, name):
        self.name = name

    def generate_output(self, output):
        output.append('Q(' + self.name + ')')

    def __str__(self):
        return 'QString-' + self.name


class Output(object):
    def __init__(self):
        self.lines = []
        self.indentation = 0

    def indent(self):
        self.indentation += 1

    def dedent(self):
        self.indentation -= 1

    def append(self, line):
        indent_string = '  ' * self.indentation
        self.lines.append(indent_string + line)

    def join(self):
        return '\n'.join(self.lines)


def remove_comments(text):
    """
    Best guess based on
      http://stackoverflow.com/questions/241327/python-snippet-to-remove-c-and-c-comments
    """
    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):
            return " "  # note: a space and not an empty string
        else:
            return s
    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )
    return re.sub(pattern, replacer, text)


def process_file(f):
    # Load the file
    file_content = f.read()
    file_content = remove_comments(file_content)

    lines = file_content.split('\n')

    state = NodeStack()

    not_terminated = None
    for line in lines:
        # Join continuations
        if not_terminated:
            line = not_terminated + ' ' + line

        line = line.strip()
        if len(line) == 0:
            continue

        # Do we need to join lines?
        if line[-1] == '\\':
            not_terminated = line[:-1].strip()
            continue
        else:
            not_terminated = None

        if line[0] == '#':
            line = '#' + line[1:].strip()

            if line.startswith('#if'):
                state.push_if(line)
                debug("Detected 'if': %s" % line)
            elif line.startswith('#elif'):
                state.push_elif(line)
                debug("Detected 'elif': %s" % line)
            elif line.startswith('#else'):
                state.push_else(line)
                debug("Detected 'else': %s" % line)
            elif line.startswith('#endif'):
                state.push_endif(line)
                debug("Detected 'endif': %s" % line)
            else:
                state.push_other_directive(line)
                debug("Detected directive: %s" % line)

        else:
            for match in re.findall(r'MP_QSTR_[_a-zA-Z0-9]+', line):
                name = match.replace('MP_QSTR_', '')
                if name not in QSTRING_BLACK_LIST:
                    qstring = QString(name)
                    debug("Found QString: %s" % qstring)
                    state.push_qstring(qstring)


    region = state.get_result()
    region.validate()
    output = Output()
    region.generate_output(output)

    return output.join() + '\n'



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Generates qstr definitions from a specified source')

    parser.add_argument('-o', '--output-file',
                        dest='output_filename',
                        help='Output filename (defaults to stdout)')
    parser.add_argument('input_filename',
                        nargs='?',
                        help='Name of the input file (when not specified, the script reads standard input')
    parser.add_argument('-s', '--skip-write-when-same', dest='skip_write_when_same', action='store_true',
                   default=False,
                   help="Skip writting into the output file if it already exists and the content is the same as the currently generated qstring definitions (disabled by default)")

    args = parser.parse_args()

    # Check if the file contents changed from last time
    write_file = True

    # By default write into STDOUT
    outfile = sys.stdout
    real_output_filename = 'STDOUT'

    if args.input_filename:
        infile = open(args.input_filename, 'r')
    else:
        infile = sys.stdin

    file_data = process_file(infile)
    infile.close()


    # Detect custom output file name
    if args.output_filename:
        real_output_filename = args.output_filename
        if os.path.isfile(args.output_filename) and args.skip_write_when_same:
            with open(args.output_filename, 'r') as f:
                existing_data = f.read()
            if existing_data == file_data:
                sys.stderr.write("Skip regeneration of: %s\n" %
                                 real_output_filename)
                write_file = False
            else:
                debug("File HAS changed, overwriting\n")
                outfile = open(args.output_filename, 'w')
        else:
            outfile = open(args.output_filename, 'w')

    # Only write the file if we the data has changed
    if write_file:
        sys.stderr.write("Generate qstrings into: %s\n" % real_output_filename)
        outfile.write(file_data)
        outfile.close()
