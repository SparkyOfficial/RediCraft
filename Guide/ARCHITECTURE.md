# RediCraft Architecture Documentation

## System Overview

RediCraft is a distributed key-value store designed specifically for Minecraft plugin development. The system consists of two main components:

1. **RediCraft-Server**: A high-performance C++ server implementing the core storage and networking functionality
2. **RediCraft-API**: A Java client library providing easy integration with Minecraft plugins

## RediCraft-Server Architecture

### Core Components

#### 1. Server Module
The server module handles incoming client connections and manages the overall server lifecycle.

**Key Responsibilities:**
- TCP connection handling using ASIO
- Client session management
- Configuration loading
- Server lifecycle management

**Files:**
- `server.h/cpp` - Main server implementation
- `session.h/cpp` - Client session handling
- `config.h/cpp` - Configuration management

#### 2. Storage Module
The storage module provides thread-safe key-value storage with support for multiple data types.

**Key Responsibilities:**
- Thread-safe data storage
- Multiple data type support (strings, hashes, lists)
- Expiration and TTL management
- Persistence integration

**Files:**
- `storage.h/cpp` - Core storage implementation
- `persistence.h/cpp` - Data persistence functionality

#### 3. Protocol Module
The protocol module handles command parsing and response formatting.

**Key Responsibilities:**
- Command parsing
- Response formatting
- Protocol implementation

**Files:**
- `parser.h/cpp` - Command parser
- `session.h/cpp` - Session handling (integrates with protocol)

#### 4. Distributed Module
The distributed module implements replication and clustering functionality.

**Key Responsibilities:**
- Master-slave replication
- Cluster management
- Node discovery
- Request routing

**Files:**
- `replication.h/cpp` - Replication implementation
- `cluster.h/cpp` - Clustering implementation

### Data Flow

```
1. Client Connection
   ↓
2. Session Creation
   ↓
3. Command Parsing
   ↓
4. Storage Operation
   ↓
5. Response Formatting
   ↓
6. Client Response
```

### Threading Model

RediCraft uses a multi-threaded architecture for optimal performance:

1. **Main Thread**: Handles server initialization and configuration
2. **IO Threads**: Multiple threads running ASIO io_context for handling network I/O
3. **Worker Threads**: Optional threads for background operations (persistence, replication)

### Data Structures

#### Storage Implementation
The storage system uses separate containers for different data types:

```cpp
// String storage
std::unordered_map<std::string, DataItem> string_data_;

// Hash storage
std::unordered_map<std::string, HashItem> hash_data_;

// List storage
std::unordered_map<std::string, ListItem> list_data_;
```

Each data item contains:
- Actual value
- Expiration timestamp
- Expiration flag

#### Thread Safety
Thread safety is implemented using `std::shared_mutex`:
- Shared locks for read operations
- Unique locks for write operations

## RediCraft-API Architecture

### Core Components

#### 1. Client Module
The client module provides the main interface for interacting with the RediCraft server.

**Key Responsibilities:**
- TCP connection management
- Command serialization
- Response deserialization
- Error handling

**Files:**
- `RedicraftClient.java` - Main client implementation

#### 2. Connection Pool Module
The connection pool module manages a pool of reusable connections for improved performance.

**Key Responsibilities:**
- Connection pooling
- Connection lifecycle management
- Thread-safe connection borrowing/returning

**Files:**
- `ConnectionPool.java` - Connection pool implementation

### Data Flow

```
1. Client Method Call
   ↓
2. Command Serialization
   ↓
3. Network Transmission
   ↓
4. Server Processing
   ↓
5. Response Reception
   ↓
6. Response Deserialization
   ↓
7. Return to Caller
```

### Asynchronous Operations

Asynchronous operations are implemented using `CompletableFuture`:

```java
public CompletableFuture<Boolean> setAsync(String key, String value) {
    return CompletableFuture.supplyAsync(() -> {
        return set(key, value);
    });
}
```

## Distributed Architecture

### Replication

#### Master-Slave Model
RediCraft implements a master-slave replication model:

1. **Master Node**: Accepts write operations and replicates data to slaves
2. **Slave Nodes**: Accept read operations and synchronize data from master

#### Replication Process
```
1. Write Operation on Master
   ↓
2. Log Replication Command
   ↓
3. Send to Slave Nodes
   ↓
4. Apply on Slave Nodes
   ↓
5. Acknowledge Completion
```

### Clustering

#### Hash Slot Distribution
RediCraft uses hash slot distribution for data sharding:

1. **Hash Calculation**: Keys are hashed to determine their slot (0-16383)
2. **Slot Assignment**: Slots are distributed among cluster nodes
3. **Request Routing**: Requests are routed to the appropriate node

#### Node Discovery
Cluster nodes automatically discover each other through:
1. Configuration-based initial node list
2. Periodic node health checks
3. Dynamic node addition/removal

## Performance Considerations

### Memory Management
- Efficient data structures (unordered_map for O(1) average access)
- Minimal memory copying
- Smart pointer usage for automatic memory management

### Concurrency
- Shared mutexes for reader-writer patterns
- Lock-free operations where possible
- Thread-local storage for per-thread data

### Network I/O
- Asynchronous I/O with ASIO
- Connection reuse through pooling
- Efficient buffer management

## Security Architecture

### Current Security Model
- Simple TCP connections
- No authentication (planned for future versions)
- No encryption (planned for future versions)

### Future Security Enhancements
- User authentication
- TLS encryption
- Access control lists
- Audit logging

## Monitoring and Observability

### Built-in Metrics
- Connection count
- Request rate
- Response times
- Memory usage
- Thread utilization

### Health Checks
- Node availability
- Cluster health
- Replication status
- Persistence status

## Scalability Design

### Horizontal Scaling
- Cluster support for sharding
- Automatic node discovery
- Load distribution
- Seamless scaling

### Vertical Scaling
- Multi-threading support
- Efficient resource utilization
- Memory optimization

## Fault Tolerance

### Replication-Based Redundancy
- Data replication across nodes
- Automatic failover
- Data consistency guarantees

### Error Handling
- Graceful degradation
- Automatic recovery
- Error logging and monitoring

## Deployment Architecture

### Single Node Deployment
```
┌─────────────────────────┐
│    RediCraft-Server     │
│      (Standalone)       │
└─────────────────────────┘
          ▲
          │
┌─────────▼─────────┐
│   RediCraft-API   │
│   (Minecraft)     │
└───────────────────┘
```

### Replicated Deployment
```
┌─────────────────┐
│  Master Node    │
│ RediCraft-Server│
└─────────┬───────┘
          │
┌─────────▼───────┐
│  Slave Node 1   │
│ RediCraft-Server│
└─────────────────┘

┌─────────────────┐
│  Slave Node 2   │
│ RediCraft-Server│
└─────────────────┘
```

### Clustered Deployment
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Node 1        │    │   Node 2        │    │   Node 3        │
│ RediCraft-Server│    │ RediCraft-Server│    │ RediCraft-Server│
│   (Slots 0-5k)  │    │  (Slots 5k-11k) │    │  (Slots 11k-16k)│
└─────────────────┘    └─────────────────┘    └─────────────────┘
          ▲                      ▲                      ▲
          └──────────────────────┼──────────────────────┘
                                 │
                    ┌────────────▼────────────┐
                    │    RediCraft-API        │
                    │   (Minecraft)           │
                    └─────────────────────────┘
```