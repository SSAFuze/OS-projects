# Shell

This project aims to replicate key shell features. It is currently a work in progress and some features may have bugs.

## Current features

- Execution of simple Linux commands
- Shell built-ins "cd" and "exit"
- Handles SIGINT so shell continues running but can use to kill child processes
- Can run command in background by placing & after it
- Support for && operator (subsequent commands only execute if previous return value is 0)
