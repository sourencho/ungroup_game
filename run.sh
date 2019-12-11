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

# You can exit all tmux windows at once by pressing CTRL+B and typing :kill-window

tmux new -s ungroup -d
tmux rename-window -t ungroup game
tmux send-keys -t ungroup './build/src/server/ug-server' C-m
tmux split-window -v -t ungroup
tmux send-keys -t ungroup './build/src/client/ug-client' C-m
tmux split-window -v -t ungroup
tmux send-keys -t ungroup './build/src/client/ug-client' C-m
tmux select-window -t ungroup:1
tmux attach -t ungroup

