TARGET=red_triangle extra_example
IDIR =../include
CC=g++
CFLAGS=-Wall -pedantic -I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS=-lGLEW -lGL -lglut

_DEPS = Angel.h CheckError.h mat.h vec.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ1 = red_triangle.o InitShader.o
_OBJ2 = extra_example.o InitShader.o
OBJ1 = $(patsubst %,$(ODIR)/%,$(_OBJ1))
OBJ2 = $(patsubst %,$(ODIR)/%,$(_OBJ2))

all: directories $(TARGET)

directories:
	mkdir -p $(ODIR)

red_triangle: $(OBJ1)
	$(CC) -o $@ $^  $(CFLAGS) $(LIBS)

extra_example: $(OBJ2)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run: all
	./red_triangle

run2: all
	./extra_example

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -rf $(ODIR) *~ core $(INCDIR)/*~ red_triangle extra_example
