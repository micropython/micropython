# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

# Top-level Makefile for documentation builds and miscellaneous tasks.
#

# You can set these variables from the command line.
PYTHON        = python3
SPHINXOPTS    = -W --keep-going
SPHINXBUILD   = sphinx-build
PAPER         =
# path to build the generated docs
BUILDDIR      = _build
# path to source files to process
SOURCEDIR     = .
# path to conf.py
CONFDIR      = .
# Run "make FORCE= ..." to avoid rebuilding from scratch (and risk
# producing incorrect docs).
FORCE         = -E
VERBOSE       = -v

# path to generated type stubs
STUBDIR       = circuitpython-stubs
# Run "make VALIDATE= stubs" to avoid validating generated stub files
VALIDATE      = -v
# path to pypi source distributions
DISTDIR       = dist

# Make sure you have Sphinx installed, then set the SPHINXBUILD environment variable to point to the
# full path of the '$(SPHINXBUILD)' executable. Alternatively you can add the directory with the
# executable to your PATH. If you don't have Sphinx installed, grab it from http://sphinx-doc.org/)

# Internal variables.
PAPEROPT_a4     = -D latex_paper_size=a4
PAPEROPT_letter = -D latex_paper_size=letter
BASEOPTS        = -c $(CONFDIR) $(PAPEROPT_$(PAPER)) $(FORCE) $(VERBOSE) $(SPHINXOPTS) $(SOURCEDIR)
ALLSPHINXOPTS   = -d $(BUILDDIR)/doctrees $(BASEOPTS)
# the i18n builder cannot share the environment and doctrees with the others
I18NSPHINXOPTS  = $(BASEOPTS)

TRANSLATE_SOURCES = extmod lib main.c ports/atmel-samd ports/cxd56 ports/esp32s2 ports/mimxrt10xx ports/nrf ports/raspberrypi ports/stm py shared-bindings shared-module supervisor
# Paths to exclude from TRANSLATE_SOURCES
# Each must be preceded by "-path"; if any wildcards, enclose in quotes.
# Separate by "-o" (Find's "or" operand)
TRANSLATE_SOURCES_EXC = -path "ports/*/build-*" \
	-o -path "ports/*/build" \
	-o -path ports/atmel-samd/asf4 \
	-o -path ports/cxd56/spresense-exported-sdk \
	-o -path ports/esp32s2/esp-idf \
	-o -path ports/mimxrt10xx/sdk \
	-o -path ports/raspberrypi/sdk \
	-o -path ports/stm/st_driver \
	-o -path lib/tinyusb \
	-o -path lib/lwip \
	-o -path extmod/ulab/circuitpython \
	-o -path extmod/ulab/micropython \

.PHONY: help clean html dirhtml singlehtml pickle json htmlhelp qthelp devhelp epub latex latexpdf text man changes linkcheck doctest gettext stubs

help:
	@echo "Please use \`make <target>' where <target> is one of"
	@echo "  html       to make standalone HTML files"
	@echo "  dirhtml    to make HTML files named index.html in directories"
	@echo "  singlehtml to make a single large HTML file"
	@echo "  pickle     to make pickle files"
	@echo "  json       to make JSON files"
	@echo "  htmlhelp   to make HTML files and a HTML help project"
	@echo "  qthelp     to make HTML files and a qthelp project"
	@echo "  devhelp    to make HTML files and a Devhelp project"
	@echo "  epub       to make an epub"
	@echo "  latex      to make LaTeX files, you can set PAPER=a4 or PAPER=letter"
	@echo "  latexpdf   to make LaTeX files and run them through pdflatex"
	@echo "  latexpdfja to make LaTeX files and run them through platex/dvipdfmx"
	@echo "  text       to make text files"
	@echo "  man        to make manual pages"
	@echo "  texinfo    to make Texinfo files"
	@echo "  info       to make Texinfo files and run them through makeinfo"
	@echo "  gettext    to make PO message catalogs"
	@echo "  changes    to make an overview of all changed/added/deprecated items"
	@echo "  xml        to make Docutils-native XML files"
	@echo "  pseudoxml  to make pseudoxml-XML files for display purposes"
	@echo "  linkcheck  to check all external links for integrity"
	@echo "  doctest    to run all doctests embedded in the documentation (if enabled)"

clean:
	rm -rf $(BUILDDIR)/*
	rm -rf autoapi
	rm -rf $(STUBDIR) $(DISTDIR) *.egg-info

html: stubs
	$(SPHINXBUILD) -b html $(ALLSPHINXOPTS) $(BUILDDIR)/html
	@echo
	@echo "Build finished. The HTML pages are in $(BUILDDIR)/html."

dirhtml:
	$(SPHINXBUILD) -b dirhtml $(ALLSPHINXOPTS) $(BUILDDIR)/dirhtml
	@echo
	@echo "Build finished. The HTML pages are in $(BUILDDIR)/dirhtml."

singlehtml:
	$(SPHINXBUILD) -b singlehtml $(ALLSPHINXOPTS) $(BUILDDIR)/singlehtml
	@echo
	@echo "Build finished. The HTML page is in $(BUILDDIR)/singlehtml."

pickle:
	$(SPHINXBUILD) -b pickle $(ALLSPHINXOPTS) $(BUILDDIR)/pickle
	@echo
	@echo "Build finished; now you can process the pickle files."

json:
	$(SPHINXBUILD) -b json $(ALLSPHINXOPTS) $(BUILDDIR)/json
	@echo
	@echo "Build finished; now you can process the JSON files."

htmlhelp:
	$(SPHINXBUILD) -b htmlhelp $(ALLSPHINXOPTS) $(BUILDDIR)/htmlhelp
	@echo
	@echo "Build finished; now you can run HTML Help Workshop with the" \
	      ".hhp project file in $(BUILDDIR)/htmlhelp."

qthelp:
	$(SPHINXBUILD) -b qthelp $(ALLSPHINXOPTS) $(BUILDDIR)/qthelp
	@echo
	@echo "Build finished; now you can run "qcollectiongenerator" with the" \
	      ".qhcp project file in $(BUILDDIR)/qthelp, like this:"
	@echo "# qcollectiongenerator $(BUILDDIR)/qthelp/MicroPython.qhcp"
	@echo "To view the help file:"
	@echo "# assistant -collectionFile $(BUILDDIR)/qthelp/MicroPython.qhc"

devhelp:
	$(SPHINXBUILD) -b devhelp $(ALLSPHINXOPTS) $(BUILDDIR)/devhelp
	@echo
	@echo "Build finished."
	@echo "To view the help file:"
	@echo "# mkdir -p $$HOME/.local/share/devhelp/MicroPython"
	@echo "# ln -s $(BUILDDIR)/devhelp $$HOME/.local/share/devhelp/MicroPython"
	@echo "# devhelp"

epub:
	$(SPHINXBUILD) -b epub $(ALLSPHINXOPTS) $(BUILDDIR)/epub
	@echo
	@echo "Build finished. The epub file is in $(BUILDDIR)/epub."

latex:
	$(SPHINXBUILD) -b latex $(ALLSPHINXOPTS) $(BUILDDIR)/latex
	@echo
	@echo "Build finished; the LaTeX files are in $(BUILDDIR)/latex."
	@echo "Run \`make' in that directory to run these through (pdf)latex" \
	      "(use \`make latexpdf' here to do that automatically)."

# seems to be malfunctioning
latexpdf:
	$(SPHINXBUILD) -b latex $(ALLSPHINXOPTS) $(BUILDDIR)/latex
	@echo "Running LaTeX files through pdflatex..."
	$(MAKE) -C $(BUILDDIR)/latex all-pdf
	@echo "pdflatex finished; the PDF files are in $(BUILDDIR)/latex."

# seems to be malfunctioning
latexpdfja:
	$(SPHINXBUILD) -b latex $(ALLSPHINXOPTS) $(BUILDDIR)/latex
	@echo "Running LaTeX files through platex and dvipdfmx..."
	$(MAKE) -C $(BUILDDIR)/latex all-pdf-ja
	@echo "pdflatex finished; the PDF files are in $(BUILDDIR)/latex."

# seems to be malfunctioning
text:
	$(SPHINXBUILD) -b text $(ALLSPHINXOPTS) $(BUILDDIR)/text
	@echo
	@echo "Build finished. The text files are in $(BUILDDIR)/text."

# seems to be malfunctioning
man:
	$(SPHINXBUILD) -b man $(ALLSPHINXOPTS) $(BUILDDIR)/man
	@echo
	@echo "Build finished. The manual pages are in $(BUILDDIR)/man."

texinfo:
	$(SPHINXBUILD) -b texinfo $(ALLSPHINXOPTS) $(BUILDDIR)/texinfo
	@echo
	@echo "Build finished. The Texinfo files are in $(BUILDDIR)/texinfo."
	@echo "Run \`make' in that directory to run these through makeinfo" \
	      "(use \`make info' here to do that automatically)."

info:
	$(SPHINXBUILD) -b texinfo $(ALLSPHINXOPTS) $(BUILDDIR)/texinfo
	@echo "Running Texinfo files through makeinfo..."
	make -C $(BUILDDIR)/texinfo info
	@echo "makeinfo finished; the Info files are in $(BUILDDIR)/texinfo."

gettext:
	$(SPHINXBUILD) -b gettext $(I18NSPHINXOPTS) $(BUILDDIR)/locale
	@echo
	@echo "Build finished. The message catalogs are in $(BUILDDIR)/locale."

changes:
	$(SPHINXBUILD) -b changes $(ALLSPHINXOPTS) $(BUILDDIR)/changes
	@echo
	@echo "The overview file is in $(BUILDDIR)/changes."

linkcheck:
	$(SPHINXBUILD) -b linkcheck $(ALLSPHINXOPTS) $(BUILDDIR)/linkcheck
	@echo
	@echo "Link check complete; look for any errors in the above output " \
	      "or in $(BUILDDIR)/linkcheck/output.txt."

doctest:
	$(SPHINXBUILD) -b doctest $(ALLSPHINXOPTS) $(BUILDDIR)/doctest
	@echo "Testing of doctests in the sources finished, look at the " \
	      "results in $(BUILDDIR)/doctest/output.txt."

xml:
	$(SPHINXBUILD) -b xml $(ALLSPHINXOPTS) $(BUILDDIR)/xml
	@echo
	@echo "Build finished. The XML files are in $(BUILDDIR)/xml."

pseudoxml:
	$(SPHINXBUILD) -b pseudoxml $(ALLSPHINXOPTS) $(BUILDDIR)/pseudoxml
	@echo
	@echo "Build finished. The pseudo-XML files are in $(BUILDDIR)/pseudoxml."

# phony target so we always run
.PHONY: all-source
all-source:

locale/circuitpython.pot: all-source
	find $(TRANSLATE_SOURCES) -type d \( $(TRANSLATE_SOURCES_EXC) \) -prune -o -type f \( -iname "*.c" -o -iname "*.h" \) -print | (LC_ALL=C sort) | xgettext -f- -L C -s --add-location=file --keyword=translate --keyword=MP_ERROR_TEXT -o - | sed -e '/"POT-Creation-Date: /d' > $@

# Historically, `make translate` updated the .pot file and ran msgmerge.
# However, this was a frequent source of merge conflicts.  Weblate can perform
# msgmerge, so make translate merely update the translation template file.
.PHONY: translate
translate: locale/circuitpython.pot

# Note that normally we rely on weblate to perform msgmerge.  This reduces the
# chance of a merge conflict between developer changes (that only add and
# remove source strings) and weblate changes (that only add and remove
# translated strings from po files).  However, in case this is legitimately
# needed we preserve a rule to do it.
.PHONY: msgmerge
msgmerge:
	for po in $(shell ls locale/*.po); do msgmerge -U $$po -s --no-fuzzy-matching --add-location=file locale/circuitpython.pot; done

merge-translate:
	git merge HEAD 1>&2 2> /dev/null; test $$? -eq 128
	rm locale/*~ || true
	git checkout --theirs -- locale/*
	make translate

.PHONY: check-translate
check-translate:
	find $(TRANSLATE_SOURCES) -type d \( $(TRANSLATE_SOURCES_EXC) \) -prune -o -type f \( -iname "*.c" -o -iname "*.h" \) -print | (LC_ALL=C sort) | xgettext -f- -L C -s --add-location=file --keyword=translate --keyword=MP_ERROR_TEXT -o circuitpython.pot.tmp -p locale
	$(PYTHON) tools/check_translations.py locale/circuitpython.pot.tmp locale/circuitpython.pot; status=$$?; rm -f locale/circuitpython.pot.tmp; exit $$status

stubs:
	@mkdir -p circuitpython-stubs
	@$(PYTHON) tools/extract_pyi.py shared-bindings/ $(STUBDIR)
	@$(PYTHON) tools/extract_pyi.py extmod/ulab/code/ $(STUBDIR)/ulab
	@$(PYTHON) tools/extract_pyi.py ports/atmel-samd/bindings $(STUBDIR)
	@$(PYTHON) tools/extract_pyi.py ports/raspberrypi/bindings $(STUBDIR)
	@$(PYTHON) setup.py -q sdist

.PHONY: check-stubs
check-stubs: stubs
	MYPYPATH=$(STUBDIR) mypy --strict $(STUBDIR)

update-frozen-libraries:
	@echo "Updating all frozen libraries to latest tagged version."
	cd frozen; for library in *; do cd $$library; ../../tools/git-checkout-latest-tag.sh; cd ..; done

one-of-each: samd21 litex mimxrt10xx nrf stm

samd21:
	$(MAKE) -C ports/atmel-samd BOARD=trinket_m0

samd51:
	$(MAKE) -C ports/atmel-samd BOARD=feather_m4_express

esp32s2:
	$(MAKE) -C ports/esp32s2 BOARD=espressif_saola_1_wroom

litex:
	$(MAKE) -C ports/litex BOARD=fomu

mimxrt10xx:
	$(MAKE) -C ports/mimxrt10xx BOARD=feather_mimxrt1011

nrf:
	$(MAKE) -C ports/nrf BOARD=feather_nrf52840_express

stm:
	$(MAKE) -C ports/stm BOARD=feather_stm32f405_express

clean-one-of-each: clean-samd21 clean-samd51 clean-esp32s2 clean-litex clean-mimxrt10xx clean-nrf clean-stm

clean-samd21:
	$(MAKE) -C ports/atmel-samd BOARD=trinket_m0 clean

clean-samd51:
	$(MAKE) -C ports/atmel-samd BOARD=feather_m4_express clean

clean-esp32s2:
	$(MAKE) -C ports/esp32s2 BOARD=espressif_saola_1_wroom clean

clean-litex:
	$(MAKE) -C ports/litex BOARD=fomu clean

clean-mimxrt10xx:
	$(MAKE) -C ports/mimxrt10xx BOARD=feather_mimxrt1011 clean

clean-nrf:
	$(MAKE) -C ports/nrf BOARD=feather_nrf52840_express clean

clean-stm:
	$(MAKE) -C ports/stm BOARD=feather_stm32f405_express clean
