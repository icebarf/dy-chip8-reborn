# Simple Makefile by rdseed (Amritpal Singh)
# Will be used in future projects
CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic
BIN=bin
TMP=tmp

# Output file
A.OUT=chip8-rb
A.OUT_VER=0.0.2-alpha

# Third Party Library Flags
LIB=sdl2
PKGCONFIG=`pkg-config --libs --cflags $(LIB)`

# Termbox - Statically Linked
termbox_cflags:=-std=c99 -Wall -Wextra -pedantic -Wno-unused-result -g -O0 -D_XOPEN_SOURCE -D_DEFAULT_SOURCE $(CFLAGS)
termbox_o:=termbox.o
termbox_h:=include/termbox2/termbox.h

# Name of object files present under src
OBJECTS=chip.o debugger.o helpers.o

.DEFAULT_GOAL: all

all: TEMP $(OBJECTS) termbox binary clean

TEMP:
	mkdir -p $(TMP)

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $(TMP)/$@

binary:
	$(CC) $(CFLAGS) $(wildcard tmp/*.o) -o $(BIN)/$(A.OUT)-$(A.OUT_VER) $(PKGCONFIG)

# Termbox is statically linked
termbox:
	git submodule init
	git submodule update
	$(CC) -DTB_IMPL -DTB_OPT_TRUECOLOR -DTB_OPT_EGC -fPIC -xc -c $(termbox_cflags) $(termbox_h) -o $(TMP)/$(termbox_o)

clean:
	rm tmp/*.o
	rm -r tmp
