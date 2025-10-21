# RediCraft Troubleshooting Guide

## Table of Contents
1. [Common Issues](#common-issues)
2. [Server Problems](#server-problems)
3. [Client Issues](#client-issues)
4. [Network and Connectivity](#network-and-connectivity)
5. [Performance Problems](#performance-problems)
6. [Data and Persistence](#data-and-persistence)
7. [Replication Issues](#replication-issues)
8. [Clustering Problems](#clustering-problems)
9. [Security Concerns](#security-concerns)
10. [Debugging Techniques](#debugging-techniques)

## Common Issues

### Installation Problems

#### CMake Configuration Failures
**Problem**: CMake fails to configure the project
```
CMake Error: Could not find CMakeLists.txt
```

**Solution**:
1. Ensure you're in the correct directory (`RediCraft-Server`)
2. Verify `CMakeLists.txt` exists
3. Check CMake version (3.10+ required)
```bash
cd RediCraft-Server
ls -la CMakeLists.txt
cmake --version
```

#### Missing ASIO Dependency
**Problem**: Build fails with ASIO-related errors
```
fatal error: asio.hpp: No such file or directory
```

**Solution**:
The project uses CMake's FetchContent to automatically download ASIO. If this fails:
1. Check internet connectivity
2. Manually install ASIO:
```bash
# On Ubuntu/Debian
sudo apt-get install libasio-dev

# On macOS with Homebrew
brew install asio
```

#### Maven Build Failures
**Problem**: Maven fails to build the Java client
```
[ERROR] Failed to execute goal on project redicraft-api
```

**Solution**:
1. Check JDK installation:
```bash
java -version
javac -version
```
2. Verify Maven installation:
```bash
mvn --version
```
3. Clean and rebuild:
```bash
mvn clean package
```

### Runtime Issues

#### Server Fails to Start
**Problem**: Server crashes on startup
```
Segmentation fault (core dumped)
```

**Solution**:
1. Check configuration file syntax
2. Verify port availability:
```bash
netstat -an | grep 7379
```
3. Run with verbose output:
```bash
./RediCraftServer.exe --verbose
```

#### Client Connection Failures
**Problem**: Client cannot connect to server
```
java.net.ConnectException: Connection refused
```

**Solution**:
1. Verify server is running
2. Check host and port configuration
3. Test network connectivity:
```bash
telnet localhost 7379
```

## Server Problems

### Startup Issues

#### Port Already in Use
**Problem**: Server fails to bind to port
```
bind: Address already in use
```

**Solution**:
1. Find processes using the port:
```bash
# On Linux/macOS
lsof -i :7379

# On Windows
netstat -ano | findstr :7379
```
2. Kill conflicting process or change port in configuration

#### Configuration File Errors
**Problem**: Server fails to parse configuration
```
Error parsing configuration file: redicraft.conf
```

**Solution**:
1. Check configuration file syntax:
```ini
# Correct syntax
host=localhost
port=7379
persistence_enabled=true
```
2. Ensure no special characters or encoding issues
3. Validate with configuration test script

#### Memory Allocation Failures
**Problem**: Server crashes with memory errors
```
std::bad_alloc
```

**Solution**:
1. Check available system memory
2. Reduce memory-intensive settings:
```ini
# In redicraft.conf
max_memory=1GB
```
3. Monitor memory usage during operation

### Runtime Crashes

#### Segmentation Faults
**Problem**: Server crashes with segmentation fault
```
Segmentation fault (core dumped)
```

**Solution**:
1. Enable core dumps for debugging:
```bash
ulimit -c unlimited
```
2. Run with debugging symbols:
```bash
gdb ./RediCraftServer
(gdb) run
# When it crashes:
(gdb) bt
```
3. Check for common causes:
   - Null pointer dereferences
   - Buffer overflows
   - Thread safety issues

#### Thread-Related Crashes
**Problem**: Concurrency-related crashes
```
double free or corruption
```

**Solution**:
1. Verify mutex usage in storage operations
2. Check for race conditions in shared data access
3. Use thread sanitizers:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DSANITIZE_THREAD=ON
```

### Performance Degradation

#### High CPU Usage
**Problem**: Server consumes excessive CPU
```
CPU usage: 95%
```

**Solution**:
1. Profile the application:
```bash
perf record ./RediCraftServer
perf report
```
2. Check for busy loops or inefficient algorithms
3. Adjust thread count:
```ini
# In redicraft.conf
thread_count=4
```

#### High Memory Usage
**Problem**: Memory consumption keeps growing
```
RSS: 2GB and increasing
```

**Solution**:
1. Monitor memory usage patterns
2. Check for memory leaks:
```bash
valgrind --leak-check=full ./RediCraftServer
```
3. Implement memory limits:
```ini
max_memory=2GB
eviction_policy=allkeys-lru
```

## Client Issues

### Connection Problems

#### Connection Refused
**Problem**: Client cannot establish connection
```
java.net.ConnectException: Connection refused: connect
```

**Solution**:
1. Verify server is running:
```bash
ps aux | grep RediCraftServer
```
2. Check network connectivity:
```bash
ping server-hostname
telnet server-hostname 7379
```
3. Verify firewall settings allow connections

#### Connection Timeouts
**Problem**: Client connections time out
```
java.net.SocketTimeoutException: Connect timed out
```

**Solution**:
1. Increase connection timeout:
```java
RedicraftClient client = new RedicraftClient();
client.setConnectionTimeout(10000); // 10 seconds
```
2. Check network latency:
```bash
ping server-hostname
traceroute server-hostname
```
3. Optimize network configuration

### Data Issues

#### Incorrect Data Types
**Problem**: Operations fail due to wrong data types
```
WRONGTYPE Operation against a key holding the wrong kind of value
```

**Solution**:
1. Check key data types before operations:
```java
// Instead of assuming type
String value = client.get("mykey");
// Check if it's actually a string before using as string

// For hashes
Map<String, String> hash = client.hgetAll("mykey");
if (!hash.isEmpty()) {
    // Key is a hash
}
```
2. Use appropriate commands for data types
3. Handle type conversion errors gracefully

#### Data Corruption
**Problem**: Retrieved data is corrupted or unexpected
```
Invalid data received
```

**Solution**:
1. Verify data integrity during transmission
2. Check for encoding issues:
```java
// Ensure consistent character encoding
String data = new String(bytes, StandardCharsets.UTF_8);
```
3. Implement data validation:
```java
public String getValidated(String key) {
    String value = client.get(key);
    if (value != null && !isValidData(value)) {
        throw new DataCorruptionException("Invalid data for key: " + key);
    }
    return value;
}
```

### Resource Management

#### Connection Leaks
**Problem**: Too many open connections
```
java.net.SocketException: Too many open files
```

**Solution**:
1. Always close connections:
```java
RedicraftClient client = new RedicraftClient();
try {
    client.connect("localhost", 7379);
    // Use client
} finally {
    client.close(); // Always close
}
```
2. Use connection pooling:
```java
ConnectionPool pool = new ConnectionPool("localhost", 7379, 10);
try {
    RedicraftClient client = pool.getConnection();
    // Use client
    pool.returnConnection(client); // Return to pool
} finally {
    pool.close(); // Close pool
}
```
3. Monitor connection count:
```bash
netstat -an | grep :7379 | wc -l
```

#### Memory Leaks
**Problem**: Client application memory usage grows over time
```
Java heap space
```

**Solution**:
1. Profile Java application:
```bash
jmap -histo <pid>
jconsole <pid>
```
2. Check for object retention:
```java
// Avoid keeping references to large objects
private List<String> largeCache; // Bad
// Use weak references or clear periodically
```
3. Implement proper cleanup:
```java
public class MyPlugin {
    private ConnectionPool pool;
    
    public void onDisable() {
        if (pool != null) {
            pool.close(); // Clean up resources
        }
    }
}
```

## Network and Connectivity

### Firewall Issues

#### Blocked Connections
**Problem**: Firewall blocks client-server communication
```
Connection timed out
```

**Solution**:
1. Check firewall rules:
```bash
# On Linux
sudo iptables -L
sudo ufw status

# On Windows
netsh advfirewall show allprofiles
```
2. Allow RediCraft ports:
```bash
# Allow TCP port 7379
sudo ufw allow 7379/tcp
```
3. Configure security groups (cloud environments)

#### Network Latency
**Problem**: High latency affects performance
```
High response times
```

**Solution**:
1. Measure network latency:
```bash
ping server-hostname
mtr server-hostname
```
2. Optimize network path
3. Use connection pooling to reduce connection overhead
4. Implement caching strategies

### DNS Resolution

#### Hostname Resolution Failures
**Problem**: Cannot resolve server hostname
```
java.net.UnknownHostException: server-hostname
```

**Solution**:
1. Check DNS configuration:
```bash
nslookup server-hostname
dig server-hostname
```
2. Use IP addresses instead of hostnames
3. Update hosts file if needed:
```bash
# On Linux/macOS
echo "192.168.1.100 server-hostname" >> /etc/hosts

# On Windows
echo "192.168.1.100 server-hostname" >> C:\Windows\System32\drivers\etc\hosts
```

### Load Balancing

#### Uneven Load Distribution
**Problem**: Some servers overloaded while others idle
```
Inconsistent performance
```

**Solution**:
1. Monitor server loads:
```bash
top
htop
```
2. Implement proper load balancing algorithms
3. Use connection pooling with multiple endpoints
4. Configure health checks for load balancer

## Performance Problems

### Bottleneck Identification

#### CPU Bottlenecks
**Problem**: CPU usage consistently high
```
CPU utilization: 90%+
```

**Solution**:
1. Profile CPU usage:
```bash
perf top
htop -p <pid>
```
2. Identify hot functions:
```bash
perf record -g ./RediCraftServer
perf report
```
3. Optimize critical paths:
   - Reduce lock contention
   - Optimize algorithms
   - Use more efficient data structures

#### I/O Bottlenecks
**Problem**: Disk I/O limiting performance
```
High disk wait times
```

**Solution**:
1. Monitor I/O performance:
```bash
iostat -x 1
iotop
```
2. Optimize persistence settings:
```ini
# Reduce persistence frequency
persistence_interval=300
```
3. Use SSD storage for better I/O performance

#### Memory Bottlenecks
**Problem**: Memory allocation slowing down operations
```
Frequent garbage collection
```

**Solution**:
1. Monitor memory usage:
```bash
free -h
vmstat 1
```
2. Tune JVM settings:
```bash
java -Xms512m -Xmx2g -XX:+UseG1GC
```
3. Optimize object allocation patterns

### Scaling Issues

#### Vertical Scaling Limits
**Problem**: Single server cannot handle load
```
Resource exhaustion
```

**Solution**:
1. Implement replication:
```ini
# Master-slave setup
replication_enabled=true
replication_role=master
```
2. Use clustering for horizontal scaling
3. Optimize resource usage per connection

#### Horizontal Scaling Challenges
**Problem**: Cluster performance issues
```
Inconsistent performance across nodes
```

**Solution**:
1. Balance hash slots evenly:
```bash
# Monitor slot distribution
cluster info
cluster slots
```
2. Optimize network between nodes
3. Implement proper sharding strategies

### Optimization Techniques

#### Caching Strategies
**Problem**: Repeated operations slow down system
```
High operation latency
```

**Solution**:
1. Implement application-level caching:
```java
private final Map<String, String> localCache = new ConcurrentHashMap<>();

public String getCached(String key) {
    String value = localCache.get(key);
    if (value == null) {
        value = client.get(key);
        if (value != null) {
            localCache.put(key, value);
        }
    }
    return value;
}
```
2. Use appropriate cache eviction policies
3. Monitor cache hit ratios

#### Connection Optimization
**Problem**: Connection overhead affects performance
```
High connection establishment time
```

**Solution**:
1. Use connection pooling:
```java
ConnectionPool pool = new ConnectionPool("localhost", 7379, 20);
```
2. Reuse connections where possible
3. Implement connection keep-alive

## Data and Persistence

### Persistence Failures

#### RDB Save Failures
**Problem**: Data not saved to disk
```
Failed to save RDB
```

**Solution**:
1. Check disk space:
```bash
df -h
```
2. Verify file permissions:
```bash
ls -la redicraft.rdb
```
3. Check persistence configuration:
```ini
persistence_enabled=true
persistence_file=redicraft.rdb
persistence_interval=60
```

#### Data Loss
**Problem**: Data missing after restart
```
Keys not found after server restart
```

**Solution**:
1. Verify persistence is enabled:
```ini
persistence_enabled=true
```
2. Check RDB file integrity:
```bash
file redicraft.rdb
```
3. Monitor save operations in logs

### Data Corruption

#### RDB File Corruption
**Problem**: Corrupted persistence file
```
Bad file format
```

**Solution**:
1. Backup strategy:
```bash
# Regular backups
cp redicraft.rdb redicraft_$(date +%Y%m%d_%H%M%S).rdb
```
2. Verify file integrity:
```bash
# Check file checksums
md5sum redicraft.rdb
```
3. Implement automatic backup rotation

#### Memory Data Corruption
**Problem**: In-memory data becomes corrupted
```
Invalid data in storage
```

**Solution**:
1. Enable memory debugging:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DSANITIZE_ADDRESS=ON
```
2. Check for buffer overflows
3. Verify thread safety in storage operations

### Backup and Recovery

#### Backup Failures
**Problem**: Backup process fails
```
Backup failed: Permission denied
```

**Solution**:
1. Check backup directory permissions:
```bash
ls -ld /backup/directory
```
2. Ensure sufficient disk space:
```bash
df -h /backup/directory
```
3. Test backup process manually:
```bash
cp redicraft.rdb /backup/test.rdb
```

#### Recovery Issues
**Problem**: Cannot restore from backup
```
Failed to load backup
```

**Solution**:
1. Verify backup file integrity:
```bash
file backup.rdb
md5sum backup.rdb
```
2. Check file permissions:
```bash
chmod 644 backup.rdb
```
3. Test recovery in staging environment first

## Replication Issues

### Master-Slave Setup

#### Slave Connection Failures
**Problem**: Slave cannot connect to master
```
Failed to connect to master
```

**Solution**:
1. Verify master is running and accessible:
```bash
telnet master-host 7379
```
2. Check replication configuration:
```ini
# Slave config
replication_enabled=true
replication_role=slave
master_host=master-host
master_port=7379
```
3. Verify network connectivity between master and slave

#### Data Synchronization Issues
**Problem**: Slave data inconsistent with master
```
Data mismatch between master and slave
```

**Solution**:
1. Monitor replication lag:
```bash
# On master
info replication
```
2. Check network latency between nodes
3. Verify replication configuration on both nodes

### Failover Problems

#### Automatic Failover Failures
**Problem**: Slave doesn't take over when master fails
```
No failover occurred
```

**Solution**:
1. Implement proper failover monitoring:
```java
// Client-side failover
public String getWithFailover(String key) {
    try {
        return masterClient.get(key);
    } catch (Exception e) {
        // Try slave
        return slaveClient.get(key);
    }
}
```
2. Configure health checks
3. Test failover procedures regularly

#### Split-Brain Scenarios
**Problem**: Multiple masters after failover
```
Data inconsistency across nodes
```

**Solution**:
1. Implement proper quorum mechanisms
2. Use consensus protocols
3. Monitor cluster state regularly

## Clustering Problems

### Node Discovery

#### Node Registration Failures
**Problem**: Nodes cannot join cluster
```
Failed to register node
```

**Solution**:
1. Check cluster configuration:
```ini
clustering_enabled=true
cluster_port=7381
```
2. Verify network connectivity between nodes
3. Check firewall settings for cluster ports

#### Node Health Detection
**Problem**: Dead nodes not detected
```
Cluster continues using dead nodes
```

**Solution**:
1. Implement proper health checks:
```cpp
void ClusterManager::pingNodes() {
    for (auto& node : nodes_) {
        if (!isNodeHealthy(node)) {
            node.is_alive = false;
        }
    }
}
```
2. Configure appropriate timeout values
3. Implement automatic node removal

### Data Distribution

#### Uneven Data Distribution
**Problem**: Some nodes overloaded with data
```
Hot spots in cluster
```

**Solution**:
1. Monitor hash slot distribution:
```bash
cluster slots
```
2. Rebalance hash slots if needed
3. Optimize key distribution patterns

#### Hash Slot Calculation Issues
**Problem**: Keys not routed correctly
```
Wrong node handling requests
```

**Solution**:
1. Verify hash slot calculation:
```cpp
size_t ClusterManager::calculateHashSlot(const std::string& key) const {
    std::hash<std::string> hasher;
    return hasher(key) % 16384; // Redis uses 16384 hash slots
}
```
2. Test key distribution patterns
3. Implement consistent hashing

### Request Routing

#### Routing Failures
**Problem**: Requests sent to wrong nodes
```
MOVED or ASK responses
```

**Solution**:
1. Implement proper redirection handling:
```java
public String getWithRedirection(String key) {
    try {
        return client.get(key);
    } catch (MovedException e) {
        // Update cluster topology
        updateNodeMapping(e.getNodeAddress());
        // Retry on correct node
        return getClientForNode(e.getNodeAddress()).get(key);
    }
}
```
2. Cache cluster topology information
3. Handle ASK responses appropriately

## Security Concerns

### Access Control

#### Unauthorized Access
**Problem**: Unauthorized clients connecting
```
Security breach detected
```

**Solution**:
1. Implement IP whitelisting:
```ini
# Future feature - planned for implementation
allowed_ips=192.168.1.0/24
```
2. Use firewall rules
3. Implement authentication (planned feature)

#### Data Exposure
**Problem**: Sensitive data accessible
```
Data leak detected
```

**Solution**:
1. Encrypt sensitive data:
```java
// Future implementation
String encrypted = encrypt(data, key);
String decrypted = decrypt(encrypted, key);
```
2. Implement access controls
3. Audit data access patterns

### Network Security

#### Man-in-the-Middle Attacks
**Problem**: Data intercepted in transit
```
Network traffic monitoring
```

**Solution**:
1. Implement TLS encryption (planned):
```ini
# Future configuration
tls_enabled=true
tls_cert_file=server.crt
tls_key_file=server.key
```
2. Use VPN for sensitive communications
3. Monitor network traffic

#### Denial of Service
**Problem**: Service overwhelmed by requests
```
Service unavailable
```

**Solution**:
1. Implement rate limiting:
```cpp
// Future implementation
if (getRequestRate(client_ip) > MAX_REQUESTS_PER_SECOND) {
    rejectRequest();
}
```
2. Use DDoS protection services
3. Implement connection limits

## Debugging Techniques

### Logging Strategies

#### Server-Side Logging
**Problem**: Insufficient information for debugging
```
Vague error messages
```

**Solution**:
1. Enable verbose logging:
```cpp
// Add detailed logging
std::cout << "Processing command: " << commandStr << std::endl;
```
2. Implement structured logging:
```cpp
logger.info("Client connected from {}", client_address);
```
3. Use appropriate log levels:
   - DEBUG: Detailed diagnostic information
   - INFO: General information
   - WARN: Warning conditions
   - ERROR: Error conditions

#### Client-Side Logging
**Problem**: Client behavior unclear
```
Client-side issues hard to trace
```

**Solution**:
1. Add logging to client operations:
```java
logger.debug("Sending command: {}", command);
String response = readResponse();
logger.debug("Received response: {}", response);
```
2. Implement request tracing
3. Log connection lifecycle events

### Profiling Tools

#### CPU Profiling
**Problem**: Performance bottlenecks unknown
```
Slow operation identification
```

**Solution**:
1. Use perf on Linux:
```bash
perf record ./RediCraftServer
perf report
```
2. Use Visual Studio Profiler on Windows
3. Analyze hot functions and call stacks

#### Memory Profiling
**Problem**: Memory usage issues
```
Memory leak detection
```

**Solution**:
1. Use Valgrind on Linux:
```bash
valgrind --leak-check=full ./RediCraftServer
```
2. Use AddressSanitizer:
```bash
cmake -DSANITIZE_ADDRESS=ON
```
3. Monitor Java heap usage:
```bash
jmap -histo <pid>
```

### Monitoring Solutions

#### Real-Time Monitoring
**Problem**: Issues detected too late
```
Reactive rather than proactive
```

**Solution**:
1. Implement health check endpoints:
```cpp
// Add health check command
if (command == "HEALTH") {
    response_ = generateHealthReport();
}
```
2. Use monitoring tools (Prometheus, Grafana)
3. Set up alerting for critical metrics

#### Metric Collection
**Problem**: No performance data available
```
Blind optimization
```

**Solution**:
1. Collect key metrics:
   - Request rate
   - Response times
   - Error rates
   - Memory usage
   - CPU utilization
2. Export metrics in standard formats
3. Store historical data for trend analysis

### Diagnostic Commands

#### Server Diagnostics
**Problem**: Server state unknown
```
Black box debugging
```

**Solution**:
1. Implement diagnostic commands:
```
INFO - Server information
CLIENT LIST - Connected clients
MEMORY USAGE - Memory statistics
SLOWLOG - Slow command log
```
2. Add custom diagnostic commands:
```cpp
// Custom diagnostics
void Session::handle_command(const std::string& commandStr) {
    if (commandStr == "DIAG") {
        response_ = generateDiagnostics();
    }
}
```

#### Client Diagnostics
**Problem**: Client behavior unclear
```
Client-side debugging difficult
```

**Solution**:
1. Add diagnostic methods:
```java
public class RedicraftClient {
    public String getDiagnostics() {
        return String.format("Connected: %s, LastError: %s, RequestCount: %d",
            isConnected(), lastError, requestCount);
    }
}
```
2. Implement connection tracing
3. Add performance counters

### Testing Strategies

#### Unit Testing
**Problem**: Code changes break existing functionality
```
Regression issues
```

**Solution**:
1. Write comprehensive unit tests:
```java
@Test
public void testSetAndGet() {
    assertTrue(client.set("test", "value"));
    assertEquals("value", client.get("test"));
}
```
2. Test edge cases
3. Use test-driven development

#### Integration Testing
**Problem**: Components don't work together
```
Integration failures
```

**Solution**:
1. Test client-server interactions:
```java
@Test
public void testServerIntegration() {
    // Start test server
    // Connect client
    // Test operations
    // Verify results
}
```
2. Test replication scenarios
3. Test clustering functionality

#### Stress Testing
**Problem**: Performance under load unknown
```
Production performance issues
```

**Solution**:
1. Create stress test scenarios:
```bash
# Generate high load
ab -n 10000 -c 100 http://localhost:7379/
```
2. Monitor resource usage under load
3. Test failover scenarios under stress

This troubleshooting guide provides comprehensive solutions for common RediCraft issues. Regular monitoring and proactive maintenance will help prevent most problems from occurring in the first place.