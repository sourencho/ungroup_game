# ungroup

[![Build Status](https://travis-ci.com/SourenP/ungroup_game.svg?branch=master)](https://travis-ci.com/SourenP/ungroup_game)

## Intro

A game about temporary alliances.
Engine written from scratch using the [SFML](https://www.sfml-dev.org/) framework.

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
4. Run server and client in seperate terminals:
```
./build/src/server/ug-server
```
```
./build/src/client/ug-client
```

## Testing

To run tests, after building, run:
```
./build/tests/ug-test
```

## Development

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

## Debugging

### Travis CI Debug Build

Run this to enable debug mode on a specific build.
```
  curl -s -X POST \
   -H "Content-Type: application/json" \
   -H "Accept: application/json" \
   -H "Travis-API-Version: 3" \
   -H "Authorization: token <TOKEN>" \
   -d '{ "quiet": true }' \
   https://api.travis-ci.com/job/<JOB_ID>/debug
```

### Vscode lldb

In vscode on a mac you can debug using `lldb`.

#### 1. Install the extention "Native Debug"

Link: https://marketplace.visualstudio.com/items?itemName=webfreak.debug

#### 2. Setup tasks

Set `tasks.json` to:
```
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "clean",
            "type": "shell",
            "options": {
                "cwd": "${workspaceRoot}"
            },
            "command": "rm -rf build",
            "problemMatcher": []
        },
        {
            "label": "dir",
            "type": "shell",
            "options": {
                "cwd": "${workspaceRoot}"
            },
            "command": "mkdir -p build",
            "problemMatcher": []
        },
        {
            "label": "cmake_debug",
            "type": "shell",
            "options": {
                "cwd": "${workspaceRoot}/build"
            },
            "command": "cmake",
            "args": [
                "-DCMAKE_BUILD_TYPE=Debug",
                "${workspaceRoot}"
            ],
            "dependsOn": [
                "dir"
            ],
            "problemMatcher": []
        },
        {
            "label": "make_debug",
            "type": "shell",
            "options": {
                "cwd": "${workspaceRoot}/build"
            },
            "command": "make -j VERBOSE=1",
            "dependsOn": [
                "cmake_debug"
            ]
        },
        {
            "label": "build_debug",
            "type": "shell",
            "options": {
                "cwd": "${workspaceRoot}/build"
            },
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "dependsOn": [
                "make_debug"
            ],
            "problemMatcher": [
                "$gcc"
            ]
        }
    ]
}
```

#### 3. Setup launch

Set `launch.json` to:
```
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(lldb) Launch ug-server",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceRoot}/build/src/server/ug-server",
            "args": [],
            "stopAtEntry": true,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "lldb"
        },
        {
            "name": "(lldb) Launch ug-client",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceRoot}/build/src/client/ug-client",
            "args": [],
            "stopAtEntry": true,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "lldb"
        }
    ]
}
```

#### 4. Run tasks

Debug build:
- `CMD` + `SHIFT` + `P` >  ` Tasks: Run Task` > `build_debug`

Debug run:
- Set breakpoints
- Run `(lldb) Launch ug-server` and/or `(lldb) Launch ug-client` via the debug tab.

## Resources

### Game Engine
- [Jason Gregory: Game Engine Architecture](https://g.co/kgs/B9LnMR)
- [Bob Nystrom: Game Programming Patterns](http://gameprogrammingpatterns.com)
    - [Singleton](http://gameprogrammingpatterns.com/singleton.html)

### Physics
- [Gafferon Games](https://web.archive.org/web/20181107181427/https://gafferongames.com)
  - [Game Physics](https://web.archive.org/web/20181107181427/https://gafferongames.com/categories/game-physics)
- [Chris Hecker](http://www.chrishecker.com)
  - [Rigid body dynamics](http://www.chrishecker.com/Rigid_Body_Dynamics)

### Networking
- [Gabriel Gambetta](https://www.gabrielgambetta.com)
  - [Fast-Paced Multiplayer](https://www.gabrielgambetta.com/client-server-game-architecture.html)
- [Gafferon Games](https://web.archive.org/web/20181107181427/https://gafferongames.com) 
  - [Networked Physics](https://web.archive.org/web/20181107181445/https://gafferongames.com/categories/networked-physics)
- [Valve: Developer Community](https://developer.valvesoftware.com)
  - [Source Multiplayer Networking](https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking)
  - [Latency Compensating Methods in Client/Server In-game Protocol Design and Optimization](https://developer.valvesoftware.com/wiki/Latency_Compensating_Methods_in_Client/Server_In-game_Protocol_Design_and_Optimization)

### Graphics

- [The Book of Shaders](https://thebookofshaders.com/)

### C++

- [lefticus: CPP Best Practices](https://github.com/lefticus/cppbestpractices)
- [cliutils: An Introduction to Modern CMake](https://cliutils.gitlab.io/modern-cmake/)

### SFML

- [SFML: Tutorials for SFML 2.5](https://www.sfml-dev.org/tutorials/2.5/)

### Misc
- @inzombiak
