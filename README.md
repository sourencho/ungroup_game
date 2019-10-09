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

## Development

- Class member variables have the style `mCamelCase>`.
- Class methods have the style `camelCase`.
- Local variables have the style `snake_case`.
- Variables with the suffix `_t` are used by multiple threads and have a corresponding `<var>_lock` mutex.
- Variables with the suffix `_ta` are used by multiple threads but don't have a mutex because they are atomic.

## Debugging

In vscode on a mac you can debug using `lldb`.

### 1. Install the extention "Native Debug"

Link: https://marketplace.visualstudio.com/items?itemName=webfreak.debug

### 2. Setup tasks

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

### 3. Setup launch

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

### 4. Run tasks

Debug build:
- `CMD` + `SHIFT` + `P` >  ` Tasks: Run Task` > `build_debug`

Debug run:
- Set breakpoints
- Run `(lldb) Launch ug-server` and/or `(lldb) Launch ug-client` via the debug tab.

## Resources

### Game Engine
- [Jason Gregory: Game Engine Architecture](https://g.co/kgs/B9LnMR)

### Physics
- [Gafferon Games: Game Physics](https://web.archive.org/web/20181107181427/https://gafferongames.com/categories/game-physics)

### Networking
- [Gabriel Gambetta: Fast-Paced Multiplayer](https://www.gabrielgambetta.com/client-server-game-architecture.html)
- [Gafferon Games: Networked Physics](https://web.archive.org/web/20181107181445/https://gafferongames.com/categories/networked-physics)
- [Valve: Source Multiplayer Networking](https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking)
- [Valve: Latency Compensating Methods in Client/Server In-game Protocol Design and Optimization](https://developer.valvesoftware.com/wiki/Latency_Compensating_Methods_in_Client/Server_In-game_Protocol_Design_and_Optimization)

## Misc
- @inzombiak
