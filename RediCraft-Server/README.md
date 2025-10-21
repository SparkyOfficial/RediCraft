# RediCraft Server

High-performance C++ key-value server for Minecraft plugins.

## Overview

RediCraft Server is a fast, in-memory key-value store designed specifically for Minecraft plugins that need low-latency data access. It uses asynchronous I/O for high concurrency and provides a simple text-based protocol for communication.

## Features

- Asynchronous TCP server using ASIO
- Thread-safe storage with shared mutexes
- Simple text-based protocol
- Support for Redis-like commands (PING, SET, GET, INCR, DECR, INCRBY, HSET, HGET, HGETALL, LPUSH, RPOP, LRANGE, EXPIRE, TTL)
- Configuration file support
- Connection pooling (client-side)

## Building

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler
- Internet connection (for fetching ASIO library)

### Build Steps

```bash
# Navigate to the server directory
cd RediCraft-Server

# Configure with CMake
cmake -B build

# Build the project
cmake --build build
```

The executable will be created in `build/Debug/RediCraftServer.exe` (on Windows) or `build/RediCraftServer` (on Unix-like systems).

A benchmark tool is also available:
```bash
# Build the benchmark tool
cmake --build build --target benchmark

# Run the benchmark
./build/Debug/benchmark.exe
```

## Configuration

The server can be configured using a `redicraft.conf` file in the same directory as the executable:

```ini
# RediCraft Server Configuration File

# Server settings
host=localhost
port=7379

# Persistence settings
persistence_enabled=true
persistence_file=redicraft.rdb
persistence_interval=60

# Performance settings
max_connections=1000
```

## Running

```bash
# Run the server on port 7379
./build/RediCraftServer
```

## Protocol

The server supports a simple text-based protocol:

### Basic Commands
- `PING` - Returns `PONG`
- `SET key value` - Sets a key-value pair
- `GET key` - Returns the value for a key

### Numeric Commands
- `INCR key` - Increments the integer value of a key by 1
- `DECR key` - Decrements the integer value of a key by 1
- `INCRBY key increment` - Increments the integer value of a key by the given amount

### Hash Commands
- `HSET key field value` - Sets a field in a hash to a value
- `HGET key field` - Returns the value of a field in a hash
- `HGETALL key` - Returns all fields and values in a hash

### List Commands
- `LPUSH key value [value ...]` - Adds values to the head of a list
- `RPOP key` - Removes and returns the last element of a list
- `LRANGE key start end` - Returns a range of elements from a list

### Expiration Commands
- `EXPIRE key seconds` - Sets a timeout on a key
- `TTL key` - Returns the time to live for a key

## Example Usage

```bash
# Connect to the server (using telnet or nc)
telnet localhost 7379

# Send commands
PING
# Response: PONG

SET player:Sparky:money 1000
# Response: OK

GET player:Sparky:money
# Response: 1000

INCR player:Sparky:kills
# Response: 1

INCR player:Sparky:kills
# Response: 2

DECR player:Sparky:kills
# Response: 1

INCRBY player:Sparky:money 500
# Response: 1500

HSET player:Sparky name Andriy
# Response: OK

HSET player:Sparky level 15
# Response: OK

HGET player:Sparky name
# Response: Andriy

HGETALL player:Sparky
# Response: name: Andriy
#          level: 15

LPUSH mylist item1 item2 item3
# Response: 3

RPOP mylist
# Response: item3

EXPIRE tempkey 60
# Response: 1

TTL tempkey
# Response: 60
```

## Performance

The current implementation achieves:
- 780,000+ SET operations per second
- 800,000+ GET operations per second
- 600,000+ INCR/DECR operations per second

## Architecture

The server is built with the following components:

1. **Network Layer** - Asynchronous TCP server using ASIO
2. **Storage Layer** - Thread-safe key-value store using `std::unordered_map` and `std::shared_mutex`
3. **Protocol Layer** - Simple parser for text-based commands
4. **Session Layer** - Handles individual client connections
5. **Configuration Layer** - Manages server settings
6. **Persistence Layer** - Handles data durability (planned)

## Future Enhancements

- Lock-free data structures for better performance
- Master-slave replication
- Sharding/clustering support
- Data persistence to disk