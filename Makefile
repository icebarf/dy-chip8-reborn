# Simple Makefile by rdseed (Amritpal Singh)
# Will be used in future projects
CC := gcc
CFLAGS += -Wall -Wextra -Wpedantic

# Directories
BIN := bin
SRC := src

# Output file
VER := 0.0.4-alpha
A.OUT := chip8-rb-$(VER)
ifeq ($(OS),Windows_NT)
	A.OUT := chip8-rb-$(VER).exe
endif

# Third Party Library Flags
LIB := sdl2
PKGCONFIG := `pkg-config --libs --cflags $(LIB)`

# Name of files present under src in order they depend on each other
OBJECTS := chip.o chip_instructions.o graphics.o helpers.o 

.PHONY: all binary clean

.DEFAULT_GOAL: all

all: $(OBJECTS) binary clean

%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -g -c $< -o $@ $(PKGCONFIG)


binary: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BIN)/$(A.OUT) $(PKGCONFIG)

clean:
	rm *.o
