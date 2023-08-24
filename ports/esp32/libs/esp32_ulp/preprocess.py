from . import nocomment
from .util import split_tokens
from .definesdb import DefinesDB


class RTC_Macros:
    @staticmethod
    def READ_RTC_REG(rtc_reg, low_bit, bit_width):
        return '\treg_rd ' + ', '.join((
            rtc_reg,
            '%s + %s - 1' % (low_bit, bit_width),
            low_bit
        ))

    @staticmethod
    def WRITE_RTC_REG(rtc_reg, low_bit, bit_width, value):
        return '\treg_wr ' + ', '.join((
            rtc_reg,
            '%s + %s - 1' % (low_bit, bit_width),
            low_bit,
            value
        ))

    @staticmethod
    def READ_RTC_FIELD(rtc_reg, low_bit):
        return RTC_Macros.READ_RTC_REG(rtc_reg, low_bit, 1)

    @staticmethod
    def WRITE_RTC_FIELD(rtc_reg, low_bit, value):
        return RTC_Macros.WRITE_RTC_REG(rtc_reg, low_bit, 1, value + ' & 1')


class Preprocessor:
    def __init__(self):
        self._defines_db = None
        self._defines = {}

    def parse_define_line(self, line):
        line = line.strip()
        if not line.startswith("#define"):
            # skip lines not containing #define
            return {}
        line = line[8:].strip()  # remove #define
        parts = line.split(None, 1)
        if len(parts) != 2:
            # skip defines without value
            return {}
        identifier, value = parts
        tmp = identifier.split('(', 1)
        if len(tmp) == 2:
            # skip parameterised defines (macros)
            return {}
        value = "".join(nocomment.remove_comments(value)).strip()
        return {identifier: value}

    def parse_defines(self, content):
        for line in content.splitlines():
            self._defines.update(self.parse_define_line(line))

        return self._defines

    def expand_defines(self, line):
        found = True
        while found:  # do as many passed as needed, until nothing was replaced anymore
            found = False
            tokens = split_tokens(line)
            line = ""
            for t in tokens:
                lu = self._defines.get(t, t)
                if lu == t and self._defines_db:
                    lu = self._defines_db.get(t, t)
                if lu == t and t == 'BIT':
                    # Special hack: BIT(..) translates to a 32-bit mask where only the specified bit is set.
                    # But the reg_wr and reg_rd opcodes expect actual bit numbers for argument 2 and 3.
                    # While the real READ_RTC_*/WRITE_RTC_* macros take in the output of BIT(x), they
                    # ultimately convert these back (via helper macros) to the bit number (x). And since this
                    # preprocessor does not (aim to) implement "proper" macro-processing, we can simply
                    # short-circuit this round-trip via macros and replace "BIT" with nothing so that
                    # "BIT(x)" gets mapped to "(x)".
                    continue
                if lu != t:
                    found = True
                line += lu

        return line

    def process_include_file(self, filename):
        with self.open_db() as db:
            with open(filename, 'r') as f:
                for line in f:
                    result = self.parse_define_line(line)
                    db.update(result)

        return db

    def expand_rtc_macros(self, line):
        clean_line = line.strip()
        if not clean_line:
            return line

        macro = clean_line.split('(', 1)
        if len(macro) != 2:
            return line

        macro_name, macro_args = macro

        macro_fn = getattr(RTC_Macros, macro_name, None)
        if macro_fn is None:
            return line

        macro_args, _ = macro_args.rsplit(')', 1)  # trim away right bracket. safe as comments already stripped
        macro_args = macro_args.split(',')  # not safe when args contain ',' but we should not have those
        macro_args = [x.strip() for x in macro_args]

        return macro_fn(*macro_args)

    def use_db(self, defines_db):
        self._defines_db = defines_db

    def open_db(self):
        class ctx:
            def __init__(self, db):
                self._db = db

            def __enter__(self):
                # not opening DefinesDB - it opens itself when needed
                return self._db

            def __exit__(self, type, value, traceback):
                if isinstance(self._db, DefinesDB):
                    self._db.close()

        if self._defines_db:
            return ctx(self._defines_db)

        return ctx(self._defines)

    def preprocess(self, content):
        self.parse_defines(content)

        with self.open_db():
            lines = nocomment.remove_comments(content)
            result = []
            for line in lines:
                line = self.expand_defines(line)
                line = self.expand_rtc_macros(line)
                result.append(line)
            result = "\n".join(result)

        return result


def preprocess(content, use_defines_db=True):
    preprocessor = Preprocessor()
    preprocessor.use_db(DefinesDB())
    return preprocessor.preprocess(content)
