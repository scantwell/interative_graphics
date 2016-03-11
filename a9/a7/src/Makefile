TARGET=a9
IDIR =../include
CC=g++
CFLAGS=-std=c++11  -Wall -pedantic -I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS=-lGLEW -lGL -lglut -lpng

_DEPS =  mesh.h Angel.h CheckError.h mat.h vec.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = run.o mesh.o InitShader.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: directories $(TARGET)

directories:
	mkdir -p $(ODIR)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^  $(CFLAGS) $(LIBS)

run: all
	./$(TARGET)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -rf $(ODIR) *~ core $(INCDIR)/*~ $(TARGET)
