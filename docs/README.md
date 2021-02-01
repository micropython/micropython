MicroPython Documentation
=========================

The MicroPython documentation can be found at:
http://docs.micropython.org/en/latest/

The documentation you see there is generated from the files in the docs tree:
https://github.com/micropython/micropython/tree/master/docs

Building the documentation locally
----------------------------------

If you're making changes to the documentation, you may want to build the
documentation locally so that you can preview your changes.

Install Sphinx, and optionally (for the RTD-styling), sphinx_rtd_theme,
preferably in a virtualenv:

     pip install sphinx
     pip install sphinx_rtd_theme

In `micropython/docs`, build the docs:

    make html

You'll find the index page at `micropython/docs/build/html/index.html`.

Having readthedocs.org build the documentation
----------------------------------------------

If you would like to have docs for forks/branches hosted on GitHub, GitLab or
BitBucket an alternative to building the docs locally is to sign up for a free
https://readthedocs.org account. The rough steps to follow are:
1. sign-up for an account, unless you already have one
2. in your account settings: add GitHub as a connected service (assuming
you have forked this repo on github)
3. in your account projects: import your forked/cloned micropython repository
into readthedocs
4. in the project's versions: add the branches you are developing on or
for which you'd like readthedocs to auto-generate docs whenever you
push a change

PDF manual generation
---------------------

This can be achieved with:

    make latexpdf

but require rather complete install of LaTeX with various extensions. On
Debian/Ubuntu, try (500MB+ download):

    apt-get install texlive-latex-recommended texlive-latex-extra
