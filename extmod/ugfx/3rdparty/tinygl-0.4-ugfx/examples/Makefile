include ../config.mk

PROGS = mech texobj gears spin

all: $(PROGS)

clean:
	rm -f core *.o *~ $(PROGS)

mech: mech.o glu.o $(UI_OBJS) $(GL_DEPS)
	$(CC) $(LFLAGS) $^ -o $@ $(GL_LIBS) $(UI_LIBS) -lm

texobj: texobj.o $(UI_OBJS) $(GL_DEPS)
	$(CC) $(LFLAGS) $^ -o $@ $(GL_LIBS) $(UI_LIBS) -lm

gears: gears.o $(UI_OBJS) $(GL_DEPS)
	$(CC) $(LFLAGS) $^ -o $@ $(GL_LIBS) $(UI_LIBS) -lm

spin: spin.o $(UI_OBJS) $(GL_DEPS)
	$(CC) $(LFLAGS) $^ -o $@ $(GL_LIBS) $(UI_LIBS) -lm

.c.o:
	$(CC)	$(CFLAGS) $(GL_INCLUDES) $(UI_INCLUDES) -c $*.c

mech.o: glu.h 

glu.o: glu.h

ui.o: ui.h
