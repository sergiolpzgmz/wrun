# wrun

**wrun** is a utility written in C to identify processes that are using a specific TCP port on Linux systems and, optionally, terminate those processes.

## Description

- Allows specifying a TCP port and displays the associated process.
- Offers an option to terminate the process listening on that port.
- Provides built-in help via standard options (`--help`, `--version`).

## Usage

Show help:

```sh
./wrun --help
```

Operate on a TCP port:

```sh
# Query the process using port 8080
./wrun -p 8080

# Terminate the process associated with the port
./wrun -k -p 8080

# Long form
./wrun --port=8080 --kill
```

> Note: The `-p/--port` parameter is required.

## Requirements

- GCC compiler
- Make utility
- Linux system

## Compile

### Using Makefile (Recommended)

```sh
cd wrun
make
```

This will compile the project and generate the `wrun` executable in the project root directory.