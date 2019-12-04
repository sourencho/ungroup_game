#!/bin/bash

cmake -S . -B build
cmake --build build -- -j

tmux new -s ungroup -d
tmux rename-window -t ungroup game
tmux send-keys -t ungroup './build/src/server/ug-server' C-m
tmux split-window -v -t ungroup
tmux send-keys -t ungroup './build/src/client/ug-client' C-m
tmux split-window -v -t ungroup
tmux send-keys -t ungroup './build/src/client/ug-client' C-m
tmux select-window -t ungroup:1
tmux attach -t ungroup

