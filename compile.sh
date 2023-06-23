#!/bin/sh
clear
set -ex
output="main.out"
libraries="-lm -lSDL2 -lSDL2_ttf -lSDL2_gfx -lSDL2_image"
flags="-std=c11 -Wall -Wextra -Wpedantic -Werror -g"
# flags="-std=c11 -Wall -Wextra -Wpedantic -Werror -Ofast"
gcc main.c  $flags $libraries -o $output
