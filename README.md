# ungroup

## Intro

A game about temporary alliances.

## Building and Running
**Unix**
1. Clone this repo
2. Install SFML-devel package
3. Configure and build:
```
mkdir build
cmake -S . -B build
cmake --build build -- -j
```
5. Run server and client in seperate terminals:
```
./build/src/server/ug-server
```
```
./build/src/client/ug-client
```


