EXAMPLES = \
	blink \
	hello_world \
	interactive \
	timer

all:
	for example in $(EXAMPLES) ; do \
	    $(MAKE) -C $$example all ; \
	done

clean:
	for example in $(EXAMPLES) ; do \
	    $(MAKE) -C $$example clean ; \
	done
