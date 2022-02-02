# What is dy-chip8-reborn

dy-chip8-reborn is the second iteration of my chip8 emulator in C. The previous/first version was a rush job and I was inexperienced and the
code base ended up being a mess with macros and what not god forbidden things littered all over the place.

This is my attempt to completely rewrite another chip8 emulator from scratch and newer goals in mind:

- Multi-threading
- Accurate timers
- Clean and Readable Code
- Portable across Operating Systems

and anything else that I may wish to do in the future.

# Usage

*To be functional yet*

# Dependencies

- [SDL2](https://libsdl.org/) - Graphics Rendering and Threading Library
- cc - A C Compiler usually GCC or Clang
- [ninja](https://ninja-build.org) - Backend used by meson
- [meson](https://mesonbuild.com/) - The Build System
- [termbox2](https://github.com/termbox/termbox2) - Shipped in the include folder as submodule

# Compiling

1. Get a copy of the code - using `git clone` or Download Zip from GitHub.
2. `cd` to the directory of code.
3. `git submodule init`
4. `git submodule update`
5. `meson compile -C build`

After following above three steps, you should have a file called `chip8-rb`.