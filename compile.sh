#!/bin/sh
clear
set -ex
input="./src/main.c"
output="./main.out"
libraries="-lm -lSDL2 -lSDL2_ttf -lSDL2_image"
flags="-std=c11 -Wall -Wextra -Wpedantic -Werror -g"
gcc $input  $flags $libraries -o $output
