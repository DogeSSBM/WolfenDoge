#!/bin/sh
clear
set -ex
output="main.out"
libraries="-lm -lSDL2 -lSDL2_ttf"
flags="-std=c11 -Wall -Wextra -Wpedantic -Werror -g"
gcc main.c  $flags $libraries -o $output
