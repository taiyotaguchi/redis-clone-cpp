# A Redis clone with a single-threaded event loop

A Redis-compatible in-memory key-value server written in C++20, with a Python client library and CLI. The server uses a single-threaded event loop over nonblocking sockets, implements the RESP2 protocol with incremental parsing for pipelined requests, and supports the core string commands (GET, SET, INCR, DECR, PING, ECHO). Built as a learning project to explore network protocols, event-driven I/O, and low-level systems programming.

## Quick start

### Prerequisites
- Linux (uses `epoll`)
- C++20 compiler (g++ 11+ or clang 14+)
- CMake 3.20+
- Python 3.10+

### Build server
```bash
git clone https://github.com/taiyotaguchi/redis-clone-cpp.git
cd redis-clone-cpp
cmake -B build
cmake --build build
```

### Run the server
```bash
./build/redis_clone_server
```

### Run the client (in another terminal)
```bash
PYTHONPATH=. python python/client.py
```
Sample session:
```
> PING
PONG
> SET hello world
OK
> GET hello
"world"
```

## Supported commands
   
- Connection: `PING`, `ECHO`
- Strings: `GET`, `SET`, `INCR`, `DECR`

## What's next

Planned but not yet implemented: AOF persistence with configurable fsync, integration tests against real Redis, benchmark suite comparing throughput and latency to Redis, and a profiling pass to identify bottlenecks.
