# Simple Makefile by rdseed (Amritpal Singh)
# Will be used in future projects
CC := gcc
CFLAGS += -Wall -Wextra -Wpedantic

# Directories
BIN := bin
SRC := src

# Output file
A.OUT := chip8-rb
VER := 0.0.2-alpha

# Third Party Library Flags
LIB := sdl2
PKGCONFIG := `pkg-config --libs --cflags $(LIB)`

# Termbox2 - Statically Linked
termbox_cflags := -std=c99 -Wall -Wextra -pedantic -Wno-unused-result -g -O0 -D_XOPEN_SOURCE -D_DEFAULT_SOURCE $(CFLAGS)
termbox_o := termbox.o
termbox_h := include/termbox2/termbox.h

# Name of files present under src in order they depend on each other
OBJECTS := debugger.o helpers.o chip.o

.PHONY: all binary clean

.DEFAULT_GOAL: all

all: $(termbox_o) $(OBJECTS) binary clean

# Termbox is statically linked
$(termbox_o): $(termbox_h)
	@git submodule init
	@git submodule update
	$(CC) -DTB_IMPL -DTB_OPT_TRUECOLOR -DTB_OPT_EGC -fPIC -xc -c $(termbox_cflags) $(termbox_h) -o $(termbox_o)

%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

OB := $(OBJECTS) $(termbox_o)

binary: $(OB)
	$(CC) $(CFLAGS) $(OB) -o $(BIN)/$(A.OUT)-$(VER) $(PKGCONFIG)

clean:
	rm *.o
