"""
Generate documentation for pyboard API from C files.
"""

import os
import argparse
import re
import markdown

# given a list of (name,regex) pairs, find the first one that matches the given line
def re_match_first(regexs, line):
    for name, regex in regexs:
        match = re.match(regex, line)
        if match:
            return name, match
    return None, None

def makedirs(d):
    if not os.path.isdir(d):
        os.makedirs(d)

class Lexer:
    class LexerError(Exception):
        pass

    class EOF(Exception):
        pass

    class Break(Exception):
        pass

    def __init__(self, file):
        self.filename = file
        with open(file, 'rt') as f:
            line_num = 0
            lines = []
            for line in f:
                line_num += 1
                line = line.strip()
                if line == '///':
                    lines.append((line_num, ''))
                elif line.startswith('/// '):
                    lines.append((line_num, line[4:]))
                elif len(lines) > 0 and lines[-1][1] is not None:
                    lines.append((line_num, None))
        if len(lines) > 0 and lines[-1][1] is not None:
            lines.append((line_num, None))
        self.cur_line = 0
        self.lines = lines

    def opt_break(self):
        if len(self.lines) > 0 and self.lines[0][1] is None:
            self.lines.pop(0)

    def next(self):
        if len(self.lines) == 0:
            raise Lexer.EOF
        else:
            l = self.lines.pop(0)
            self.cur_line = l[0]
            if l[1] is None:
                raise Lexer.Break
            else:
                return l[1]

    def error(self, msg):
        print('({}:{}) {}'.format(self.filename, self.cur_line, msg))
        raise Lexer.LexerError

class DocValidateError(Exception):
    pass

class DocItem:
    def __init__(self):
        self.doc = []

    def add_doc(self, lex):
        try:
            while True:
                line = lex.next()
                if len(line) > 0 or len(self.doc) > 0:
                    self.doc.append(line)
        except Lexer.Break:
            pass

    def dump(self):
        return '\n'.join(self.doc)

class DocConstant(DocItem):
    def __init__(self, name, descr):
        super().__init__()
        self.name = name
        self.descr = descr

    def dump(self, ctx):
        return '`{}.{}` - {}'.format(ctx, self.name, self.descr)

class DocFunction(DocItem):
    def __init__(self, name, args):
        super().__init__()
        self.name = name
        self.args = args

    def dump(self, ctx):
        if self.name == '\\constructor':
            s = '### `{}{}`'.format(ctx, self.args)
        elif self.name == '\\call':
            s = '### `{}{}`'.format(ctx, self.args)
        else:
            s = '### `{}.{}{}`'.format(ctx, self.name, self.args)
        return s + '\n' + super().dump()

class DocClass(DocItem):
    def __init__(self, name, descr):
        super().__init__()
        self.name = name
        self.descr = descr
        self.constructors = {}
        self.classmethods = {}
        self.methods = {}
        self.constants = {}

    def process_classmethod(self, lex, d):
        name = d['id']
        if name == '\\constructor':
            dict_ = self.constructors
        else:
            dict_ = self.classmethods
        if name in dict_:
            lex.error("multiple definition of method '{}'".format(name))
        method = dict_[name] = DocFunction(name, d['args'])
        method.add_doc(lex)

    def process_method(self, lex, d):
        name = d['id']
        dict_ = self.methods
        if name in dict_:
            lex.error("multiple definition of method '{}'".format(name))
        method = dict_[name] = DocFunction(name, d['args'])
        method.add_doc(lex)

    def process_constant(self, lex, d):
        name = d['id']
        if name in self.constants:
            lex.error("multiple definition of constant '{}'".format(name))
        self.constants[name] = DocConstant(name, d['descr'])
        lex.opt_break()

    def dump(self):
        s = []
        s.append('')
        s.append('# class {}'.format(self.name))
        s.append('')
        s.append(super().dump())
        if len(self.constructors) > 0:
            s.append('')
            s.append("## Constructors")
            for f in sorted(self.constructors.values(), key=lambda x:x.name):
                s.append('')
                s.append(f.dump(self.name))
        if len(self.classmethods) > 0:
            s.append('')
            s.append("## Class methods")
            for f in sorted(self.classmethods.values(), key=lambda x:x.name):
                s.append('')
                s.append(f.dump(self.name))
        if len(self.methods) > 0:
            s.append('')
            s.append("## Methods")
            for f in sorted(self.methods.values(), key=lambda x:x.name):
                s.append('')
                s.append(f.dump(self.name.lower()))
        if len(self.constants) > 0:
            s.append('')
            s.append("## Constants")
            for c in sorted(self.constants.values(), key=lambda x:x.name):
                s.append('')
                s.append(c.dump(self.name))
        return '\n'.join(s)

class DocModule(DocItem):
    def __init__(self, name, descr):
        super().__init__()
        self.name = name
        self.descr = descr
        self.functions = {}
        self.constants = {}
        self.classes = {}
        self.cur_class = None

    def new_file(self):
        self.cur_class = None

    def process_function(self, lex, d):
        name = d['id']
        if name in self.functions:
            lex.error("multiple definition of function '{}'".format(name))
        function = self.functions[name] = DocFunction(name, d['args'])
        function.add_doc(lex)

    #def process_classref(self, lex, d):
    #    name = d['id']
    #    self.classes[name] = name
    #    lex.opt_break()

    def process_class(self, lex, d):
        name = d['id']
        if name in self.classes:
            lex.error("multiple definition of class '{}'".format(name))
        self.cur_class = self.classes[name] = DocClass(name, d['descr'])
        self.cur_class.add_doc(lex)

    def process_classmethod(self, lex, d):
        self.cur_class.process_classmethod(lex, d)

    def process_method(self, lex, d):
        self.cur_class.process_method(lex, d)

    def process_constant(self, lex, d):
        if self.cur_class is None:
            # a module-level constant
            name = d['id']
            if name in self.constants:
                lex.error("multiple definition of constant '{}'".format(name))
            self.constants[name] = DocConstant(name, d['descr'])
            lex.opt_break()
        else:
            # a class-level constant
            self.cur_class.process_constant(lex, d)

    def validate(self):
        if self.descr is None:
            raise DocValidateError('module {} referenced but never defined'.format(self.name))

    def dump(self):
        s = []
        s.append('# module {}'.format(self.name))
        s.append('')
        s.append(super().dump())
        if self.functions:
            s.append('')
            s.append('## Functions')
            for f in sorted(self.functions.values(), key=lambda x:x.name):
                s.append('')
                s.append(f.dump(self.name))
        if self.constants:
            s.append('')
            s.append("## Constants")
            for c in sorted(self.constants.values(), key=lambda x:x.name):
                s.append('')
                s.append(c.dump(self.name))
        if self.classes:
            s.append('')
            s.append('## Classes')
            for c in sorted(self.classes.values(), key=lambda x:x.name):
                s.append('')
                s.append('[`{}.{}`]({}) - {}'.format(self.name, c.name, c.name, c.descr))
        return '\n'.join(s)

    def write(self, dir):
        index = markdown.markdown(self.dump())
        with open(os.path.join(dir, 'index.html'), 'wt') as f:
            f.write(index)
        for c in self.classes.values():
            class_dir = os.path.join(dir, c.name)
            makedirs(class_dir)
            class_dump = c.dump()
            class_dump = 'part of the [{} module](./)'.format(self.name) + '\n' + class_dump
            index = markdown.markdown(class_dump)
            with open(os.path.join(class_dir, 'index.html'), 'wt') as f:
                f.write(index)

class Doc:
    def __init__(self):
        self.modules = {}
        self.cur_module = None

    def new_file(self):
        self.cur_module = None
        for m in self.modules.values():
            m.new_file()

    def check_module(self, lex):
        if self.cur_module is None:
            lex.error('module not defined')

    def process_module(self, lex, d):
        name = d['id']
        if name not in self.modules:
            self.modules[name] = DocModule(name, None)
        self.cur_module = self.modules[name]
        if self.cur_module.descr is not None:
            lex.error("multiple definition of module '{}'".format(name))
        self.cur_module.descr = d['descr']
        self.cur_module.add_doc(lex)

    def process_moduleref(self, lex, d):
        name = d['id']
        if name not in self.modules:
            self.modules[name] = DocModule(name, None)
        self.cur_module = self.modules[name]
        lex.opt_break()

    def process_class(self, lex, d):
        self.check_module(lex)
        self.cur_module.process_class(lex, d)

    def process_function(self, lex, d):
        self.check_module(lex)
        self.cur_module.process_function(lex, d)

    def process_classmethod(self, lex, d):
        self.check_module(lex)
        self.cur_module.process_classmethod(lex, d)

    def process_method(self, lex, d):
        self.check_module(lex)
        self.cur_module.process_method(lex, d)

    def process_constant(self, lex, d):
        self.check_module(lex)
        self.cur_module.process_constant(lex, d)

    def validate(self):
        for m in self.modules.values():
            m.validate()

    def dump(self):
        s = []
        s.append('# Modules')
        s.append('')
        s.append('These are the Python modules that are implemented.')
        s.append('')
        for m in sorted(self.modules.values(), key=lambda x:x.name):
            s.append('')
            s.append('[`{}`]({}/) - {}'.format(m.name, m.name, m.descr))
        return '\n'.join(s)

    def write(self, dir):
        with open(os.path.join(dir, 'module', 'index.html'), 'wt') as f:
            f.write(markdown.markdown(self.dump()))
        for m in self.modules.values():
            mod_dir = os.path.join(dir, 'module', m.name)
            makedirs(mod_dir)
            m.write(mod_dir)

regex_descr = r'(?P<descr>.*)'

doc_regexs = (
    (Doc.process_module, re.compile(r'\\module (?P<id>[a-z]+) - ' + regex_descr + r'$')),
    (Doc.process_moduleref, re.compile(r'\\moduleref (?P<id>[a-z]+)$')),
    (Doc.process_function, re.compile(r'\\function (?P<id>[a-z0-9_]+)(?P<args>\(.*\))$')),
    (Doc.process_classmethod, re.compile(r'\\classmethod (?P<id>\\?[a-z0-9_]+)(?P<args>\(.*\))$')),
    (Doc.process_method, re.compile(r'\\method (?P<id>\\?[a-z0-9_]+)(?P<args>\(.*\))$')),
    (Doc.process_constant, re.compile(r'\\constant (?P<id>[A-Za-z0-9_]+) - ' + regex_descr + r'$')),
    #(Doc.process_classref, re.compile(r'\\classref (?P<id>[A-Za-z0-9_]+)$')),
    (Doc.process_class, re.compile(r'\\class (?P<id>[A-Za-z0-9_]+) - ' + regex_descr + r'$')),
)

def process_file(file, doc):
    lex = Lexer(file)
    doc.new_file()
    try:
        try:
            while True:
                line = lex.next()
                fun, match = re_match_first(doc_regexs, line)
                if fun == None:
                    lex.error('unknown line format: {}'.format(line))
                fun(doc, lex, match.groupdict())

        except Lexer.Break:
            lex.error('unexpected break')

        except Lexer.EOF:
            pass

    except Lexer.LexerError:
        return False

    return True

def main():
    cmd_parser = argparse.ArgumentParser(description='Generate documentation for pyboard API from C files.')
    cmd_parser.add_argument('--outdir', metavar='<output dir>', default='gendoc-out', help='ouput directory')
    cmd_parser.add_argument('files', nargs='+', help='input files')
    args = cmd_parser.parse_args()

    doc = Doc()
    for file in args.files:
        print('processing', file)
        if not process_file(file, doc):
            return
    try:
        doc.validate()
    except DocValidateError as e:
        print(e)
    doc.write(args.outdir)
    print('written to', args.outdir)

if __name__ == "__main__":
    main()
