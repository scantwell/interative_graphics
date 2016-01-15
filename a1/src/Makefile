IDIR =../include
CC=g++
CFLAGS=-Wall -pedantic -I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS=-lGLEW -lGL -lglut

_DEPS = Angel.h CheckError.h mat.h vec.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = red_triangle.o InitShader.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


red_triangle: $(OBJ)
	$(CC) -o $@ $^  $(CFLAGS) $(LIBS)

run: red_triangle 
	./red_triangle

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ red_triangle
