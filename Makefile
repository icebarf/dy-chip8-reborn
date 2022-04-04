# Simple Makefile by rdseed (Amritpal Singh)
# Will be used in future projects
CC := gcc
CFLAGS += -Wall -Wextra -Wpedantic

# Directories
BIN := bin
SRC := src

# Release mode and flags
DEBUG := -g3 -fsanitize=thread
RELEASE := -O2 -fomit-frame-pointer -flto
MODE:=$(RELEASE)

# Output file
VER := 0.0.7-beta
A.OUT := chip8-rb-$(VER)
ifeq ($(OS),Windows_NT)
	A.OUT := chip8-rb-$(VER).exe
endif

# Third Party Library Flags
LIB := sdl2
PKGCONFIG := `pkg-config --libs --cflags $(LIB)`

# Name of files present under src in order they depend on each other
OBJECTS := chip.o chip_instructions.o graphics.o helpers.o keyboard.o

.PHONY: all binary dir clean

.DEFAULT_GOAL: all

all: dir $(OBJECTS) binary

dir:
	mkdir -p bin

%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(MODE) -c $< -o $(BIN)/$@ $(PKGCONFIG)


binary: $(BIN)/$(OBJECTS)
	$(CC) $(CFLAGS) $(BIN)/*.o $(MODE) -o $(BIN)/$(A.OUT) $(PKGCONFIG)

clean:
	rm $(BIN)/*.o
