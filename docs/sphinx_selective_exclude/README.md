Sphinx eager ".. only::" directive and other selective rendition extensions
===========================================================================

Project home page: https://github.com/pfalcon/sphinx_selective_exclude

The implementation of ".. only::" directive in Sphinx documentation
generation tool is known to violate principles of least user surprise
and user expectations in general. Instead of excluding content early
in the pipeline (pre-processor style), Sphinx defers exclusion until
output phase, and what's the worst, various stages processing ignore
"only" blocks and their exclusion status, so they may leak unexpected
information into ToC, indexes, etc.

There's multiple issues submitted upstream on this matter:

* https://github.com/sphinx-doc/sphinx/issues/2150
* https://github.com/sphinx-doc/sphinx/issues/1717
* https://github.com/sphinx-doc/sphinx/issues/1488
* etc.

They are largely ignored by Sphinx maintainers.

This projects tries to rectify situation on users' side. It actually
changes the way Sphinx processes "only" directive, but does this
without forking the project, and instead is made as a standard
Sphinx extension, which a user may add to their documentation config.
Unlike normal extensions, extensions provided in this package
monkey-patch Sphinx core to work in a way expected by users.

eager_only
----------

The core extension provided by the package is called `eager_only` and
is based on the idea by Andrea Cassioli (see bugreports above) to
process "only" directive as soon as possible during parsing phase.
This approach has some drawbacks, like producing warnings like
"WARNING: document isn't included in any toctree" if "only" is used
to shape up a toctree, or the fact that changing a documentation
builder (html/latex/etc.) will almost certainly require complete
rebuild of documentation. But these are relatively minor issues
comparing to completely broken way "only" works in upstream Sphinx.

modindex_exclude
----------------

"only" directive allows for fine-grained conditional exclusion, but
sometimes you may want to exclude entire module(s) at once. Even if
you wrap an entire module description in "only" directive, like:

    .. only: option1
        .. module:: my_module
    
        ...

You will still have an HTML page generated, albeit empty. It may also
go into indexes, so will be discoverable by users, leading to less
than ideal experience. `modindex_exclude` extension is design to
resolve this issue, by making sure that any reference of a module
is excluded from Python module index ("modindex"), as well as
general cross-reference index ("genindex"). In the latter case,
any symbol belong to a module will be excluded. Unlike `eager_only`
extension which appear to have issued with "latexpdf" builder,
`modindex_exclude` is useful for PDF, and allows to get cleaner
index for PDF, just the same as for HTML.

search_auto_exclude
-------------------

Even if you exclude some documents from toctree:: using only::
directive, they will be indexed for full-text search, so user may
find them and get confused. This plugin follows very simple idea
that if you didn't include some documents in the toctree, then
you didn't want them to be accessible (e.g. for a particular
configuration), and so will make sure they aren't indexed either.

This extension depends on `eager_only` and won't work without it.
Note that Sphinx will issue warnings, as usual, for any documents
not included in a toctree. This is considered a feature, and gives
you a chance to check that document exclusions are indeed right
for a particular configuration you build (and not that you forgot
to add something to a toctree).

Summary
-------

Based on the above, sphinx_selective_exclude offers extension to let
you:

* Make "only::" directive work in an expected, intuitive manner, using
  `eager_only` extension.
* However, if you apply only:: to toctree::, excluded documents will
  still be available via full-text search, so you need to use
  `search_auto_exclude` for that to work as expected.
* Similar to search, indexes may also require special treatment, hence
  there's the `modindex_exclude` extension.

Most likely, you will want to use all 3 extensions together - if you
really want build subsets of docimentation covering sufficiently different
configurations from a single doctree. However, if one of them is enough
to cover your usecase, that's OK to (and why they were separated into
3 extensions, to follow KISS and "least surprise" principles and to
not make people deal with things they aren't interested in). In this case,
however remember there're other extensions, if you later hit a usecase
when they're needed.

Usage
-----

To use these extensions, add https://github.com/pfalcon/sphinx_selective_exclude
as a git submodule to your project, in documentation folder (where
Sphinx conf.py is located). Alternatively, commit sphinx_selective_exclude
directory instead of making it a submodule (you will need to pick up
any project updates manually then).

Add following lines to "extensions" settings in your conf.py (you
likely already have some standard Sphinx extensions enabled):

    extensions = [
        ...
        'sphinx_selective_exclude.eager_only',
        'sphinx_selective_exclude.search_auto_exclude',
        'sphinx_selective_exclude.modindex_exclude',
    ]

As discussed above, you may enable all extensions, or one by one.

Please note that to make sure these extensions work well and avoid producing
output docs with artifacts, it is IMPERATIVE to remove cached doctree if
you rebuild documentation with another builder (i.e. with different output
format). Also, to stay on safe side, it's recommended to remove old doctree
anyway before generating production-ready documentation for publishing. To
do that, run something like:

    rm -rf _build/doctrees/

A typical artificat when not following these simple rules is that content
of some sections may be missing. If you face anything like that, just
remember what's written above and remove cached doctrees.
