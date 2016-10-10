.PHONY: all doxygen sphinx clean

all:
	$(MAKE) doxygen
	$(MAKE) sphinx

doxygen:
	doxygen doxygen.cfg

sphinx:
	$(MAKE) -f sphinx.mk html
	echo "Run 'firefox _build/html/index.html' to view the generated documentation."

sphinx-clean:
	$(MAKE) -f sphinx.mk clean

clean:
	rm -rf xml
	$(MAKE) -f sphinx.mk clean
