#!/bin/sh

# Dependecy Checks
printf "Performing Dependency Checks...\n"

if ! [ -x "$(command -v git)" ]; then
    printf "Dependency: 'git' - not installed\n"
    exit 1
fi

printf "Depedency: 'git'  - installed\n"

if ! [ -x "$(command -v make)" ]; then
    printf "Dependency: 'make' - not installed\n"
    exit 1
fi

printf "Depedency: 'make' - installed\n"

if ! [ -e "/usr/include/SDL2" ]; then
    printf "Dependency: 'SDL2' - not installed\n"
    exit 1
fi

if ! [ -e "/usr/include/SDL2" ]; then
    printf "File: 'SDL2/SDL.h' - not found\n"
    exit 1
fi
printf "Depedency: 'SDL2' - installed\n"

printf "\n"

# Update Submodules 
printf "Updating submodules\n"

printf "Termbox2...\n"
$(git submodule init)
$(git submodule update)
printf "Submodule update successful\n"