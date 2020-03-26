# ungroup

[![Build Status](https://travis-ci.com/SourenP/ungroup_game.svg?branch=master)](https://travis-ci.com/SourenP/ungroup_game)

![screenshot](https://user-images.githubusercontent.com/3184499/72916611-5438c880-3d10-11ea-846b-57579872f243.png)

## Intro

A multiplayer game where the main mechanic to win is forming temporary alliances.

- The game is played in rounds.
- Each player spawns as a circle and has a unique set of resources they need to collect to win.
- Resources are collected from mines by bumping into them.
- The first player to reach their goal of resources wins.
- Players can form groups with other players by merging their circles into a group.
- The larger a group is the faster it moves/mines and when mining resources are divided among members.
- A player may leave a group and become a solo circle at any time.

Custom engine written in C++11 using the [SFML](https://www.sfml-dev.org/) framework.

Created by [@sourenp](https://github.com/SourenP) and [@copacetic](https://github.com/copacetic) mostly for learning purposes.

## Building and Running


### Unix
1. Clone this repo
2. Install requirements
  - [SFML 2.5.1](https://www.sfml-dev.org/download/sfml/2.5.1/)
  - [Boost 1.65](https://www.boost.org/)
3. Build:
```
./scripts/build.sh
```
4. Run server and client in seperate terminals:
```
./build/src/server/ug-server
```
```
./build/src/client/ug-client
```

*Note: if you are running from a remote server, you might need to open the tcp and udp ports on your machine*

## Testing

To run tests, after building, run:
```
./build/tests/ug-test
```

## Game Engine

Custom engine written in C++11 using the [SFML](https://www.sfml-dev.org/) framework.

Features:
- Game functionality
  - Variable time step
  - Unique ID system for game objects
- Physics
  - Velocity with lerping
  - 2D Circle collision detection and handling
  - 2D Circle perfect ellastic collision impulses
- Networking
  - Server-Client seperation
  - Authoritative server
  - Multi threaded UDP/TCP communication between server and client
  - Reliable (TCP) and Unreliable (UDP) client inputs sent to server
  - Unreliable (UDP) game state broadcasted to client
  - Works locally and over the internet 
- Interpolation
  - Snapshot interpolation
- Rendering
  - 2D Cirlce shapes, textures and shaders
  - Animations with texture sprite sheets
  - Parallax background
  - Variable window size support and zooming/scaling
  - Camera centering
- Resources
  - Resource manager for textures, fonts, and shaders
- Levelling
  - Configurable levels
- UI
  - Configurable UI elements allignable to window edges 
- Bots
  - Template for writing bots that query game state and input moves as a client
- Events
  - Custom event objects with data
  - Subscribe callback to event
- Metrics
  - Track temporal metrics
- Util
  - 2D vector operations

## Development

### Playtesting

Create a new [release](https://github.com/SourenP/ungroup_game/releases) and write notes in [Wiki: Playtesting](https://github.com/SourenP/ungroup_game/wiki/Playtesting)

### Compiliation

- When you create/delete a file you need to add the `<new_file>.cpp` to the corresponding `CMakeLists.txt` file.

### Writing Tests

- We use [Catch2](https://github.com/catchorg/Catch2) for testing.
- Add tests for a file in a mirror directory under `tests/`.
- Test file should be named `tests-<FileName>.cpp`
- Add a [test tag](https://github.com/catchorg/Catch2/blob/67b4ada6b0fbe98368df934e1378aeae1ba7f235/docs/test-cases-and-sections.md#tags) for each subdirectory and the file name.

### Style
- We use [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html). You can use the [xaver.clang-format](https://marketplace.visualstudio.com/items?itemName=xaver.clang-format) VSCode extension to automatically format.
- Class member variables have the style `m_camelCase`.
- Class methods have the style `camelCase`.
- Local variables have the style `snake_case`.
- Variables with the suffix `_t` are used by multiple threads and have a corresponding `<var>_lock` mutex.
- Variables with the suffix `_ta` are used by multiple threads but don't have a mutex because they are atomic.

### Debugging

#### Compile in debug mode

To compile in debug mode add the flag `-DCMAKE_BUILD_TYPE=Debug`:
```
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build
cmake --build build -- -j
```

Compiling in debug mode will give you stacktraces for segfaults and detect memory issues.

#### Profiling

Use [gperftools](https://github.com/gperftools/gperftools) ([cpuprofile tutorial](https://gperftools.github.io/gperftools/cpuprofile.html))

#### Vscode lldb

In vscode on a mac you can debug using `lldb`.

##### 1. Install the extention "Native Debug"

Link: https://marketplace.visualstudio.com/items?itemName=webfreak.debug

##### 2. Setup tasks

Set `tasks.json` to: https://gist.github.com/SourenP/46d3f5282de7fd7ecaf681384fc8e4dc

##### 3. Setup launch

Set `launch.json` to: https://gist.github.com/SourenP/47746e0c2b39545975f8e7768e281849

##### 4. Run tasks

Debug build:
- `CMD` + `SHIFT` + `P` >  ` Tasks: Run Task` > `build_debug`

Debug run:
- Set breakpoints
- Run `(lldb) Launch ug-server` and/or `(lldb) Launch ug-client` via the debug tab.

#### Travis CI Debug Build

Run this command to enable debug mode on a specific build: https://gist.github.com/SourenP/3ae15ba0e634a3e0ccceebbb7a27a391


## Resources

### Game Engine
- [Jason Gregory: Game Engine Architecture](https://g.co/kgs/B9LnMR)
- [Bob Nystrom: Game Programming Patterns](http://gameprogrammingpatterns.com)
    - [Singleton](http://gameprogrammingpatterns.com/singleton.html)

### Physics
- [Gafferon Games](https://gafferongames.com)
  - [Game Physics](https://gafferongames.com/categories/game-physics)
- [Chris Hecker](http://www.chrishecker.com)
  - [Rigid body dynamics](http://www.chrishecker.com/Rigid_Body_Dynamics)

### Networking
- [Gabriel Gambetta](https://www.gabrielgambetta.com)
  - [Fast-Paced Multiplayer](https://www.gabrielgambetta.com/client-server-game-architecture.html)
- [Gafferon Games](https://gafferongames.com)
  - [Networked Physics](https://gafferongames.com/categories/networked-physics)
- [Valve: Developer Community](https://developer.valvesoftware.com)
  - [Source Multiplayer Networking](https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking)
  - [Latency Compensating Methods in Client/Server In-game Protocol Design and Optimization](https://developer.valvesoftware.com/wiki/Latency_Compensating_Methods_in_Client/Server_In-game_Protocol_Design_and_Optimization)
- [webdva](https://webdva.github.io/)
  - [How I Implemented Client-Side Linear Interpolation](https://webdva.github.io/how-i-implemented-client-side-linear-interpolation/)

### Graphics

- [The Book of Shaders](https://thebookofshaders.com/)

### C++

- [lefticus: CPP Best Practices](https://github.com/lefticus/cppbestpractices)
- [cliutils: An Introduction to Modern CMake](https://cliutils.gitlab.io/modern-cmake/)
- [rigtorp: Awesome Modern C++](https://github.com/rigtorp/awesome-modern-cpp)

### SFML

- [SFML: Tutorials for SFML 2.5](https://www.sfml-dev.org/tutorials/2.5/)

### Misc
- [@inzombiak](https://github.com/inzombiak)
