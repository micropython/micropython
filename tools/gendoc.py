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

class MarkdownWriter:
    def __init__(self):
        pass

    def start(self):
        self.lines = []

    def end(self):
        return '\n'.join(self.lines)

    def heading(self, level, text):
        if len(self.lines) > 0:
            self.lines.append('')
        self.lines.append(level * '#' + ' ' + text)
        self.lines.append('')

    def para(self, text):
        if len(self.lines) > 0 and self.lines[-1] != '':
            self.lines.append('')
        if isinstance(text, list):
            self.lines.extend(text)
        elif isinstance(text, str):
            self.lines.append(text)
        else:
            assert False
        self.lines.append('')

    def single_line(self, text):
        self.lines.append(text)

    def module(self, name, short_descr, descr):
        self.heading(1, 'module {}'.format(name))
        self.para(descr)

    def function(self, ctx, name, args, descr):
        proto = '{}.{}{}'.format(ctx, self.name, self.args)
        self.heading(3, '`' + proto + '`')
        self.para(descr)

    def method(self, ctx, name, args, descr):
        if name == '\\constructor':
            proto = '{}{}'.format(ctx, args)
        elif name == '\\call':
            proto = '{}{}'.format(ctx, args)
        else:
            proto = '{}.{}{}'.format(ctx, name, args)
        self.heading(3, '`' + proto + '`')
        self.para(descr)

    def constant(self, ctx, name, descr):
        self.single_line('`{}.{}` - {}'.format(ctx, name, descr))

class ReStructuredTextWriter:
    head_chars = {1:'=', 2:'-', 3:'.'}

    def __init__(self):
        pass

    def start(self):
        self.lines = []

    def end(self):
        return '\n'.join(self.lines)

    def _convert(self, text):
        return text.replace('`', '``').replace('*', '\\*')

    def heading(self, level, text, convert=True):
        if len(self.lines) > 0:
            self.lines.append('')
        if convert:
            text = self._convert(text)
        self.lines.append(text)
        self.lines.append(len(text) * self.head_chars[level])
        self.lines.append('')

    def para(self, text, indent=''):
        if len(self.lines) > 0 and self.lines[-1] != '':
            self.lines.append('')
        if isinstance(text, list):
            for t in text:
                self.lines.append(indent + self._convert(t))
        elif isinstance(text, str):
            self.lines.append(indent + self._convert(text))
        else:
            assert False
        self.lines.append('')

    def single_line(self, text):
        self.lines.append(self._convert(text))

    def module(self, name, short_descr, descr):
        self.heading(1, ':mod:`{}` --- {}'.format(name, self._convert(short_descr)), convert=False)
        self.lines.append('.. module:: {}'.format(name))
        self.lines.append('   :synopsis: {}'.format(short_descr))
        self.para(descr)

    def function(self, ctx, name, args, descr):
        args = self._convert(args)
        self.lines.append('.. function:: ' + name + args)
        self.para(descr, indent='   ')

    def method(self, ctx, name, args, descr):
        args = self._convert(args)
        if name == '\\constructor':
            self.lines.append('.. class:: ' + ctx + args)
        elif name == '\\call':
            self.lines.append('.. method:: ' + ctx + args)
        else:
            self.lines.append('.. method:: ' + ctx + '.' + name + args)
        self.para(descr, indent='   ')

    def constant(self, ctx, name, descr):
        self.lines.append('.. data:: ' + name)
        self.para(descr, indent='   ')

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

    def dump(self, writer):
        writer.para(self.doc)

class DocConstant(DocItem):
    def __init__(self, name, descr):
        super().__init__()
        self.name = name
        self.descr = descr

    def dump(self, ctx, writer):
        writer.constant(ctx, self.name, self.descr)

class DocFunction(DocItem):
    def __init__(self, name, args):
        super().__init__()
        self.name = name
        self.args = args

    def dump(self, ctx, writer):
        writer.function(ctx, self.name, self.args, self.doc)

class DocMethod(DocItem):
    def __init__(self, name, args):
        super().__init__()
        self.name = name
        self.args = args

    def dump(self, ctx, writer):
        writer.method(ctx, self.name, self.args, self.doc)

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
        method = dict_[name] = DocMethod(name, d['args'])
        method.add_doc(lex)

    def process_method(self, lex, d):
        name = d['id']
        dict_ = self.methods
        if name in dict_:
            lex.error("multiple definition of method '{}'".format(name))
        method = dict_[name] = DocMethod(name, d['args'])
        method.add_doc(lex)

    def process_constant(self, lex, d):
        name = d['id']
        if name in self.constants:
            lex.error("multiple definition of constant '{}'".format(name))
        self.constants[name] = DocConstant(name, d['descr'])
        lex.opt_break()

    def dump(self, writer):
        writer.heading(1, 'class {}'.format(self.name))
        super().dump(writer)
        if len(self.constructors) > 0:
            writer.heading(2, 'Constructors')
            for f in sorted(self.constructors.values(), key=lambda x:x.name):
                f.dump(self.name, writer)
        if len(self.classmethods) > 0:
            writer.heading(2, 'Class methods')
            for f in sorted(self.classmethods.values(), key=lambda x:x.name):
                f.dump(self.name, writer)
        if len(self.methods) > 0:
            writer.heading(2, 'Methods')
            for f in sorted(self.methods.values(), key=lambda x:x.name):
                f.dump(self.name.lower(), writer)
        if len(self.constants) > 0:
            writer.heading(2, 'Constants')
            for c in sorted(self.constants.values(), key=lambda x:x.name):
                c.dump(self.name, writer)

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

    def dump(self, writer):
        writer.module(self.name, self.descr, self.doc)
        if self.functions:
            writer.heading(2, 'Functions')
            for f in sorted(self.functions.values(), key=lambda x:x.name):
                f.dump(self.name, writer)
        if self.constants:
            writer.heading(2, 'Constants')
            for c in sorted(self.constants.values(), key=lambda x:x.name):
                c.dump(self.name, writer)
        if self.classes:
            writer.heading(2, 'Classes')
            for c in sorted(self.classes.values(), key=lambda x:x.name):
                writer.para('[`{}.{}`]({}) - {}'.format(self.name, c.name, c.name, c.descr))

    def write_html(self, dir):
        md_writer = MarkdownWriter()
        md_writer.start()
        self.dump(md_writer)
        with open(os.path.join(dir, 'index.html'), 'wt') as f:
            f.write(markdown.markdown(md_writer.end()))
        for c in self.classes.values():
            class_dir = os.path.join(dir, c.name)
            makedirs(class_dir)
            md_writer.start()
            md_writer.para('part of the [{} module](./)'.format(self.name))
            c.dump(md_writer)
            with open(os.path.join(class_dir, 'index.html'), 'wt') as f:
                f.write(markdown.markdown(md_writer.end()))

    def write_rst(self, dir):
        rst_writer = ReStructuredTextWriter()
        rst_writer.start()
        self.dump(rst_writer)
        with open(dir + '/' + self.name + '.rst', 'wt') as f:
            f.write(rst_writer.end())
        for c in self.classes.values():
            rst_writer.start()
            c.dump(rst_writer)
            with open(dir + '/' + self.name + '.' + c.name + '.rst', 'wt') as f:
                f.write(rst_writer.end())

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

    def dump(self, writer):
        writer.heading(1, 'Modules')
        writer.para('These are the Python modules that are implemented.')
        for m in sorted(self.modules.values(), key=lambda x:x.name):
            writer.para('[`{}`]({}/) - {}'.format(m.name, m.name, m.descr))

    def write_html(self, dir):
        md_writer = MarkdownWriter()
        with open(os.path.join(dir, 'module', 'index.html'), 'wt') as f:
            md_writer.start()
            self.dump(md_writer)
            f.write(markdown.markdown(md_writer.end()))
        for m in self.modules.values():
            mod_dir = os.path.join(dir, 'module', m.name)
            makedirs(mod_dir)
            m.write_html(mod_dir)

    def write_rst(self, dir):
        #with open(os.path.join(dir, 'module', 'index.html'), 'wt') as f:
        #    f.write(markdown.markdown(self.dump()))
        for m in self.modules.values():
            m.write_rst(dir)

regex_descr = r'(?P<descr>.*)'

doc_regexs = (
    (Doc.process_module, re.compile(r'\\module (?P<id>[a-z][a-z0-9]*) - ' + regex_descr + r'$')),
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
    cmd_parser.add_argument('--format', default='html', help='output format: html or rst')
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

    makedirs(args.outdir)

    if args.format == 'html':
        doc.write_html(args.outdir)
    elif args.format == 'rst':
        doc.write_rst(args.outdir)
    else:
        print('unknown format:', args.format)
        return

    print('written to', args.outdir)

if __name__ == "__main__":
    main()
