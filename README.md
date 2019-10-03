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
            "label": "cmake",
            "type": "shell",
            "options": {
                "cwd": "${workspaceRoot}/build"
            },
            "command": "cmake",
            "args": [
                "${workspaceRoot}"
            ],
            "dependsOn": [
                "dir"
            ]
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
            "label": "make",
            "type": "shell",
            "options": {
                "cwd": "${workspaceRoot}/build"
            },
            "command": "make -j",
            "dependsOn": [
                "cmake"
            ]
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
            "label": "build",
            "type": "shell",
            "options": {
                "cwd": "${workspaceRoot}/build"
            },
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "dependsOn": [
                "make"
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
        }
    ]
}
```

### 4. Run tasks

Build the task:
`CMD` + `SHIFT` + `P` >  ` Tasks: Run Task` > `build_debug`

Set breakpoints and run via the bug tab.