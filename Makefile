.PHONY: clean

VER := 0.0.8-beta

ifeq ($(OS),Windows_NT)
    BIN := chip8-rb-$(VER).exe
else
    BIN := chip8-rb-$(VER)
endif

CFLAGS += -Wall -Wextra -Wpedantic -std=c2x

# Release mode and flags
ifeq ($(DEBUG),1)
	CFLAGS += -g3 -fsanitize=thread,undefined
else
	CFLAGS += -O3 -flto
endif

# Third Party Library Flags
CFLAGS += `sdl2-config --cflags`
LDFLAGS += `sdl2-config --libs`

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
