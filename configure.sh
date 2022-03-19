#!/bin/sh

 Update Submodules 
printf "Updating submodules\n"

printf "Termbox2...\n"
$(git submodule init)
$(git submodule update)
printf "Submodule update successful\n"
