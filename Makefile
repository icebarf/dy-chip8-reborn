.PHONY: clean

CC := gcc

ifeq ($(OS),Windows_NT)
    BIN := chip8-rb.exe
else
    BIN := chip8-rb
endif

CFLAGS += -Wall -Wextra -std=c2x -Wno-stringop-truncation

DEBUG:=0
# Release mode and flags
ifeq ($(DEBUG),1)
	CFLAGS += -g3 -fsanitize=address,undefined
else
	CFLAGS += -O3
endif

# Third Party Library Flags
CFLAGS += $$(sdl2-config --cflags)
LDFLAGS += $$(sdl2-config --libs)

# Static or dynamic linking
STATICBIN=0
ifeq ($(STATICBIN),1)
	LDFLAGS += -static $$(sdl2-config --libs)
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
