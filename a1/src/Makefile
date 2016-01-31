TARGET=red_triangle
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

all: directories $(TARGET)

directories:
	mkdir -p $(ODIR)

red_triangle: $(OBJ)
	$(CC) -o $@ $^  $(CFLAGS) $(LIBS)

run: all
	./red_triangle

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -rf $(ODIR) *~ core $(INCDIR)/*~ red_triangle
