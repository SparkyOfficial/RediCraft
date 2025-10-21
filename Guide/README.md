# RediCraft Comprehensive Guide

## Table of Contents
1. [Introduction](#introduction)
2. [Architecture Overview](#architecture-overview)
3. [Installation and Setup](#installation-and-setup)
4. [Server Configuration](#server-configuration)
5. [Client Usage](#client-usage)
6. [API Reference](#api-reference)
7. [Distributed Features](#distributed-features)
8. [Performance Optimization](#performance-optimization)
9. [Troubleshooting](#troubleshooting)
10. [Contributing](#contributing)

## Introduction

RediCraft is a high-performance, distributed key-value store specifically designed for Minecraft plugin development. It provides Redis-like functionality with additional features tailored for game server applications.

### Key Features
- **High Performance**: Asynchronous I/O with ASIO and multi-threading
- **Distributed System**: Master-slave replication and clustering support
- **Data Persistence**: Automatic data saving and loading
- **Minecraft Integration**: Specialized client library for Minecraft plugins
- **Easy Configuration**: Flexible configuration system

## Architecture Overview

### Components
1. **RediCraft-Server**: C++ high-performance key-value store
2. **RediCraft-API**: Java client library for Minecraft plugins

### System Architecture
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Minecraft     │    │   Minecraft     │    │   Minecraft     │
│    Server 1     │    │    Server 2     │    │    Server N     │
└─────────┬───────┘    └─────────┬───────┘    └─────────┬───────┘
          │                      │                      │
          └──────────────────────┼──────────────────────┘
                                 │
                    ┌────────────▼────────────┐
                    │    RediCraft-API        │
                    │   (Java Client)         │
                    └────────────┬────────────┘
                                 │
                    ┌────────────▼────────────┐
                    │   RediCraft-Server      │
                    │    (C++ Server)         │
                    └─────────────────────────┘
```

## Installation and Setup

### Prerequisites
- **C++ Server**: CMake 3.10+, C++17 compiler
- **Java Client**: JDK 8+, Maven 3.6+

### Building the Server
```bash
cd RediCraft-Server
cmake -B build
cmake --build build
```

### Building the Client
```bash
cd RediCraft-API
mvn clean package
```

### Running the Server
```bash
cd RediCraft-Server/build/Debug
./RediCraftServer.exe
```

## Server Configuration

### Configuration File
The server can be configured using `redicraft.conf`:

```ini
# Server settings
host=localhost
port=7379

# Persistence settings
persistence_enabled=true
persistence_file=redicraft.rdb
persistence_interval=60

# Replication settings
replication_enabled=false
replication_role=master
replication_port=7380
master_host=localhost
master_port=7379

# Clustering settings
clustering_enabled=false
cluster_port=7381
```

## Client Usage

### Basic Usage
```java
RedicraftClient client = new RedicraftClient();
client.connect("localhost", 7379);

// Set a value
client.set("key", "value");

// Get a value
String value = client.get("key");

client.close();
```

### Asynchronous Operations
```java
RedicraftClient client = new RedicraftClient();
client.connect("localhost", 7379);

// Asynchronous set
CompletableFuture<Boolean> future = client.setAsync("key", "value");
future.thenAccept(result -> {
    System.out.println("Set operation completed: " + result);
});

client.close();
```

### Connection Pooling
```java
ConnectionPool pool = new ConnectionPool("localhost", 7379, 10);

// Get connection from pool
RedicraftClient client = pool.getConnection();

// Use the client
client.set("key", "value");

// Return connection to pool
pool.returnConnection(client);
```

## API Reference

### String Operations
- `set(key, value)` - Set a key-value pair
- `get(key)` - Get the value of a key
- `incr(key)` - Increment the integer value of a key
- `decr(key)` - Decrement the integer value of a key
- `incrBy(key, increment)` - Increment the integer value of a key by the given amount

### Hash Operations
- `hset(key, field, value)` - Set a hash field
- `hget(key, field)` - Get a hash field
- `hgetAll(key)` - Get all fields and values in a hash

### List Operations
- `lpush(key, values...)` - Push values to the head of a list
- `rpop(key)` - Remove and get the last element of a list
- `lrange(key, start, end)` - Get a range of elements from a list

### Expiration Operations
- `expire(key, seconds)` - Set a key's time to live in seconds
- `ttl(key)` - Get the time to live for a key

## Distributed Features

### Replication
RediCraft supports master-slave replication for data redundancy and failover.

#### Master Configuration
```ini
replication_enabled=true
replication_role=master
replication_port=7380
```

#### Slave Configuration
```ini
replication_enabled=true
replication_role=slave
master_host=master-server
master_port=7379
```

### Clustering
RediCraft supports horizontal scaling through clustering.

#### Cluster Configuration
```ini
clustering_enabled=true
cluster_port=7381
```

## Performance Optimization

### Server Tuning
- Adjust thread count based on CPU cores
- Configure persistence interval based on data importance
- Use connection pooling for high-concurrency scenarios

### Client Tuning
- Use connection pooling for multiple concurrent operations
- Leverage asynchronous operations for non-blocking calls
- Reuse connections when possible

## Troubleshooting

### Common Issues

#### Connection Refused
- Ensure the server is running
- Check host and port configuration
- Verify firewall settings

#### Performance Issues
- Monitor CPU and memory usage
- Check thread count configuration
- Review persistence settings

### Logs and Monitoring
The server outputs logs to stdout/stderr. For production deployments, consider redirecting output to log files.

## Contributing

### Development Setup
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

### Code Style
- Follow existing code patterns
- Add comments in Ukrainian with author name "Андрій Будильников"
- Keep comments minimal and casual
- Write documentation in Russian, Ukrainian, and English