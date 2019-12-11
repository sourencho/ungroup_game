#!/bin/bash

if [[ $(which ninja) ]]; then
    echo "Detected Ninja is installed. Using Ninja."
    ninja -C build
else
    echo "Ninja not found. Using CMake directly."
    mkdir build
    cmake -S . -B build
    cmake --build build -- -j
fi
