from . import bar
from .. import bar
from ... import bar
from .... import bar
from ..... import bar
from ...... import bar
from . import bar as abc
from .foo import bar
from ..foo import bar
from ...foo import bar
from .foo.bar import baz
from ..foo.bar import baz
from ...foo.bar import baz
from .foo.bar import baz as abc
