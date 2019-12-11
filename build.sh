#!/bin/bash

if [[ $(which ninja) ]]; then
    echo "Detected Ninja is installed. Using Ninja."
    cmake -GNinja -S . -B build
    ninja -C build
else
    echo "Ninja not found. Using CMake directly."
    cmake -S . -B build
    cmake --build build -- -j
fi
