include config.mk

all:
	( for f in $(DIRS); do ( cd $$f ; make all ) || exit 1 ; done )

clean:
	rm -f *~ lib/libTinyGL.a include/GL/*~ TAGS
	( for f in $(DIRS); do ( cd $$f ; make clean ; ) done )

install:
	( for f in $(DIRS); do ( cd $$f ; make install ; ) done )


tar:
	( cd .. ; tar zcvf TinyGL-0.4.tar.gz TinyGL --exclude CVS --exclude TAGS )
