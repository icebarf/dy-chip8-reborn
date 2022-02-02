project('chip8-rb', 'c',
        version : '0.0.1-alpha',
        default_options : ['c_std=c11'] )

deps = [dependency('threads'), dependency('sdl2')]

executable('chip8-rb', ['src/chip.c','src/helpers.c','src/debugger.c'], dependencies : deps)