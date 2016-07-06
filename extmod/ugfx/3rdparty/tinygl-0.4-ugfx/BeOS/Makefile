OBJS=GLView.o 
INCLUDES = -I../include
LIB = libGLView.a

all: $(LIB)

$(LIB): $(OBJS)
	rm -f $(LIB)
	ar rcs $(LIB) $(OBJS)
	cp $(LIB) ../lib

clean:
	rm -f *~ *.o *.a

GLView.o: GLView.cpp GLView.h
	$(CC) $(CFLAGS) $(INCLUDES) -c GLView.cpp
