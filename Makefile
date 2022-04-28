.PHONY: clean

CC := gcc

ifeq ($(OS),Windows_NT)
    BIN := chip8-rb.exe
else
    BIN := chip8-rb
endif

CFLAGS += -Wall -Wextra -Wpedantic

DEBUG:=0
# Release mode and flags
ifeq ($(DEBUG),1)
	CFLAGS += -g3 -fsanitize=address,undefined
else
	CFLAGS += -O3 -DDEBUG
endif

# Third Party Library Flags
CFLAGS += $$(sdl2-config --cflags) -Iinclude/termbox2

# Static or dynamic linking
STATICBIN=0
ifeq ($(STATICBIN),1)
	LDFLAGS += -static $$(sdl2-config --libs) -Linclude/termbox2 -ltermbox
else
	LDFLAGS += $$(sdl2-config --libs) -Linclude/termbox2 -ltermbox
endif

OBJ = \
	src/chip.o \
	src/graphics.o \
	src/helpers.o \
	src/keyboard.o

# Track header file dependency changes
DEP = $(OBJ:.o=.d)
-include $(DEP)

all: $(BIN)

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -MD -c $< -o $@

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(BIN) $(DEP) $(OBJ)
