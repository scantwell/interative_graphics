TARGET=a3
IDIR =../include
CC=g++
CFLAGS=-std=c++11  -Wall -pedantic -I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS=-lGLEW -lGL -lglut

_DEPS =  Imodel.h circle.h triangle.h ellipse.h multi_square.h shape_factory.h mesh.h Angel.h CheckError.h mat.h vec.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = run.o shape_factory.o circle.o triangle.o ellipse.o multi_square.o mesh.o InitShader.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: directories $(TARGET)

directories:
	mkdir -p $(ODIR)

a3: $(OBJ)
	$(CC) -o $@ $^  $(CFLAGS) $(LIBS)

run: all
	./a3

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -rf $(ODIR) *~ core $(INCDIR)/*~ a3
