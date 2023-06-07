#!/bin/sh
clear
set -ex
./compile.sh
./main.out "$@"
