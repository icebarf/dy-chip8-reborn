project('chip8-rb', 'c',
        version : '0.0.1-alpha',
        default_options : ['c_std=c11'] )

deps = [dependency('threads'), dependency('sdl2')]

executable('chip8-rb', ['src/chip.c'], dependencies : deps)