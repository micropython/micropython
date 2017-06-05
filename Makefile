NAME=micropython
VERSION=0.0.0
RELEASE=0.0.0


help:
	@echo "make {doc}"


doc: SPHINXENV=PROJECT=$(NAME) VERSION=$(VERSION) RELEASE=$(RELEASE)
doc: doc-check-doctools
	$(SPHINXENV) sphinx-build -b html doc $(PWD)/build/doc/html
	$(SPHINXENV) sphinx-build -b singlehtml doc $(PWD)/build/doc/singlehtml
	@echo "Generated documentation under $(PWD)/build/doc/html and $(PWD)/build/doc/singlehtml"

doc-check-doctools:
	@which sphinx-build >& /dev/null || { echo "* missing sphinx-build from python sphinx package" >&2; exit 1; }



.PHONY: doc
