# RediCraft Usage Guide

## Table of Contents
1. [Getting Started](#getting-started)
2. [Server Deployment](#server-deployment)
3. [Client Integration](#client-integration)
4. [Minecraft Plugin Examples](#minecraft-plugin-examples)
5. [Advanced Configuration](#advanced-configuration)
6. [Performance Tuning](#performance-tuning)
7. [Monitoring and Maintenance](#monitoring-and-maintenance)

## Getting Started

### Prerequisites
Before using RediCraft, ensure you have the following installed:
- **For Server**: CMake 3.10+, C++17 compatible compiler
- **For Client**: JDK 8+, Maven 3.6+

### Quick Start
1. Clone the repository
2. Build the server and client
3. Configure the server
4. Start the server
5. Connect with the client

## Server Deployment

### Building the Server
```bash
cd RediCraft-Server
cmake -B build
cmake --build build
```

### Server Configuration
Create or modify `redicraft.conf` in the server directory:

```ini
# Basic server configuration
host=localhost
port=7379

# Persistence settings
persistence_enabled=true
persistence_file=redicraft.rdb
persistence_interval=60

# Performance settings
max_connections=1000
```

### Starting the Server
```bash
cd RediCraft-Server/build/Debug
./RediCraftServer.exe
```

### Server Startup Scripts
The project includes several startup scripts:
- `run_server.bat` - Starts a basic server
- `run_master.bat` - Starts a replication master
- `run_slave.bat` - Starts a replication slave
- `run_cluster.bat` - Starts a cluster node

## Client Integration

### Adding RediCraft to Your Project

#### Maven Dependency
Add the following to your `pom.xml`:
```xml
<dependency>
    <groupId>com.redicraft</groupId>
    <artifactId>redicraft-api</artifactId>
    <version>1.0-SNAPSHOT</version>
</dependency>
```

#### Manual JAR Usage
Add `redicraft-api-1.0-SNAPSHOT.jar` to your classpath.

### Basic Client Usage

#### Simple Connection
```java
import com.redicraft.RedicraftClient;

public class BasicExample {
    public static void main(String[] args) {
        RedicraftClient client = new RedicraftClient();
        
        try {
            // Connect to server
            if (client.connect("localhost", 7379)) {
                System.out.println("Connected to RediCraft server");
                
                // Set a value
                client.set("greeting", "Hello, RediCraft!");
                
                // Get a value
                String greeting = client.get("greeting");
                System.out.println("Greeting: " + greeting);
                
                // Test server responsiveness
                if (client.ping()) {
                    System.out.println("Server is responsive");
                }
            } else {
                System.err.println("Failed to connect to server");
            }
        } finally {
            // Always close the connection
            client.close();
        }
    }
}
```

#### Asynchronous Operations
```java
import com.redicraft.RedicraftClient;
import java.util.concurrent.CompletableFuture;

public class AsyncExample {
    public static void main(String[] args) {
        RedicraftClient client = new RedicraftClient();
        
        try {
            if (client.connect("localhost", 7379)) {
                // Asynchronous set operation
                CompletableFuture<Boolean> setFuture = client.setAsync("async_key", "async_value");
                
                // Handle the result
                setFuture.thenAccept(result -> {
                    System.out.println("Set operation completed: " + result);
                    
                    // Chain another operation
                    CompletableFuture<String> getFuture = client.getAsync("async_key");
                    getFuture.thenAccept(value -> {
                        System.out.println("Retrieved value: " + value);
                    });
                }).exceptionally(throwable -> {
                    System.err.println("Operation failed: " + throwable.getMessage());
                    return null;
                });
                
                // Wait a bit for async operations to complete
                Thread.sleep(1000);
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            client.close();
        }
    }
}
```

### Connection Pooling
For high-concurrency applications, use connection pooling:

```java
import com.redicraft.ConnectionPool;
import com.redicraft.RedicraftClient;

public class ConnectionPoolExample {
    public static void main(String[] args) {
        // Create a connection pool with 10 connections
        ConnectionPool pool = new ConnectionPool("localhost", 7379, 10);
        
        try {
            // Get a connection from the pool
            RedicraftClient client = pool.getConnection();
            
            try {
                // Use the client
                client.set("pooled_key", "pooled_value");
                String value = client.get("pooled_key");
                System.out.println("Value: " + value);
            } finally {
                // Return the connection to the pool
                pool.returnConnection(client);
            }
        } finally {
            // Close the pool when done
            pool.close();
        }
    }
}
```

## Minecraft Plugin Examples

### Money System Plugin
```java
import com.redicraft.RedicraftClient;
import org.bukkit.plugin.java.JavaPlugin;
import org.bukkit.command.Command;
import org.bukkit.command.CommandExecutor;
import org.bukkit.command.CommandSender;
import org.bukkit.entity.Player;

public class MoneyPlugin extends JavaPlugin {
    private RedicraftClient client;
    
    @Override
    public void onEnable() {
        // Initialize RediCraft client
        client = new RedicraftClient();
        if (!client.connect("localhost", 7379)) {
            getLogger().severe("Failed to connect to RediCraft server!");
            getServer().getPluginManager().disablePlugin(this);
            return;
        }
        
        // Register commands
        this.getCommand("balance").setExecutor(new BalanceCommand());
        this.getCommand("pay").setExecutor(new PayCommand());
        
        getLogger().info("MoneyPlugin enabled with RediCraft integration");
    }
    
    @Override
    public void onDisable() {
        if (client != null) {
            client.close();
        }
        getLogger().info("MoneyPlugin disabled");
    }
    
    private class BalanceCommand implements CommandExecutor {
        @Override
        public boolean onCommand(CommandSender sender, Command command, String label, String[] args) {
            if (!(sender instanceof Player)) {
                sender.sendMessage("Only players can check their balance!");
                return true;
            }
            
            Player player = (Player) sender;
            String balanceKey = "money:" + player.getUniqueId().toString();
            
            try {
                String balanceStr = client.get(balanceKey);
                long balance = balanceStr != null ? Long.parseLong(balanceStr) : 0;
                player.sendMessage("Your balance: $" + balance);
            } catch (Exception e) {
                player.sendMessage("Error retrieving balance: " + e.getMessage());
            }
            
            return true;
        }
    }
    
    private class PayCommand implements CommandExecutor {
        @Override
        public boolean onCommand(CommandSender sender, Command command, String label, String[] args) {
            if (!(sender instanceof Player)) {
                sender.sendMessage("Only players can send money!");
                return true;
            }
            
            if (args.length != 2) {
                sender.sendMessage("Usage: /pay <player> <amount>");
                return true;
            }
            
            Player player = (Player) sender;
            String targetPlayerName = args[0];
            Player targetPlayer = getServer().getPlayer(targetPlayerName);
            
            if (targetPlayer == null) {
                player.sendMessage("Player not found: " + targetPlayerName);
                return true;
            }
            
            try {
                long amount = Long.parseLong(args[1]);
                if (amount <= 0) {
                    player.sendMessage("Amount must be positive!");
                    return true;
                }
                
                String senderKey = "money:" + player.getUniqueId().toString();
                String targetKey = "money:" + targetPlayer.getUniqueId().toString();
                
                // Get sender's balance
                String senderBalanceStr = client.get(senderKey);
                long senderBalance = senderBalanceStr != null ? Long.parseLong(senderBalanceStr) : 0;
                
                if (senderBalance < amount) {
                    player.sendMessage("Insufficient funds! Your balance: $" + senderBalance);
                    return true;
                }
                
                // Perform transaction
                client.decrBy(senderKey, amount);
                client.incrBy(targetKey, amount);
                
                player.sendMessage("Sent $" + amount + " to " + targetPlayerName);
                targetPlayer.sendMessage("Received $" + amount + " from " + player.getName());
                
            } catch (NumberFormatException e) {
                player.sendMessage("Invalid amount: " + args[1]);
            } catch (Exception e) {
                player.sendMessage("Error processing payment: " + e.getMessage());
            }
            
            return true;
        }
    }
}
```

### Leaderboard Plugin
```java
import com.redicraft.RedicraftClient;
import org.bukkit.plugin.java.JavaPlugin;
import org.bukkit.command.Command;
import org.bukkit.command.CommandExecutor;
import org.bukkit.command.CommandSender;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;

public class LeaderboardPlugin extends JavaPlugin {
    private RedicraftClient client;
    
    @Override
    public void onEnable() {
        client = new RedicraftClient();
        if (!client.connect("localhost", 7379)) {
            getLogger().severe("Failed to connect to RediCraft server!");
            getServer().getPluginManager().disablePlugin(this);
            return;
        }
        
        this.getCommand("leaderboard").setExecutor(new LeaderboardCommand());
        getLogger().info("LeaderboardPlugin enabled");
    }
    
    @Override
    public void onDisable() {
        if (client != null) {
            client.close();
        }
    }
    
    public void updatePlayerScore(String playerName, int score) {
        String key = "leaderboard:scores";
        client.hset(key, playerName, String.valueOf(score));
    }
    
    public List<Map.Entry<String, Integer>> getTopPlayers(int count) {
        String key = "leaderboard:scores";
        Map<String, String> scores = client.hgetAll(key);
        
        List<Map.Entry<String, Integer>> sortedScores = new ArrayList<>();
        for (Map.Entry<String, String> entry : scores.entrySet()) {
            try {
                sortedScores.add(new AbstractMap.SimpleEntry<>(
                    entry.getKey(), 
                    Integer.parseInt(entry.getValue())
                ));
            } catch (NumberFormatException e) {
                // Skip invalid scores
            }
        }
        
        // Sort by score (descending)
        sortedScores.sort((a, b) -> b.getValue().compareTo(a.getValue()));
        
        // Return top count players
        return sortedScores.subList(0, Math.min(count, sortedScores.size()));
    }
    
    private class LeaderboardCommand implements CommandExecutor {
        @Override
        public boolean onCommand(CommandSender sender, Command command, String label, String[] args) {
            int count = 10;
            if (args.length > 0) {
                try {
                    count = Integer.parseInt(args[0]);
                } catch (NumberFormatException e) {
                    sender.sendMessage("Invalid count: " + args[0]);
                    return true;
                }
            }
            
            List<Map.Entry<String, Integer>> topPlayers = getTopPlayers(count);
            
            sender.sendMessage("=== Top " + topPlayers.size() + " Players ===");
            for (int i = 0; i < topPlayers.size(); i++) {
                Map.Entry<String, Integer> entry = topPlayers.get(i);
                sender.sendMessage((i + 1) + ". " + entry.getKey() + ": " + entry.getValue() + " points");
            }
            
            return true;
        }
    }
}
```

## Advanced Configuration

### Replication Setup

#### Master Configuration
```ini
# redicraft.conf for master node
host=master-server
port=7379

# Replication settings
replication_enabled=true
replication_role=master
replication_port=7380

# Persistence
persistence_enabled=true
persistence_file=redicraft.rdb
persistence_interval=60
```

#### Slave Configuration
```ini
# redicraft.conf for slave node
host=slave-server
port=7379

# Replication settings
replication_enabled=true
replication_role=slave
master_host=master-server
master_port=7379

# Persistence
persistence_enabled=true
persistence_file=redicraft.rdb
persistence_interval=60
```

### Clustering Setup
```ini
# redicraft.conf for cluster node
host=node1-server
port=7379

# Clustering settings
clustering_enabled=true
cluster_port=7381

# Persistence
persistence_enabled=true
persistence_file=redicraft.rdb
persistence_interval=60
```

### Performance Tuning Configuration
```ini
# High-performance configuration
host=localhost
port=7379

# Performance settings
max_connections=5000
thread_count=16

# Persistence settings
persistence_enabled=true
persistence_file=redicraft.rdb
persistence_interval=30

# Memory optimization
max_memory=2GB
eviction_policy=allkeys-lru
```

## Performance Tuning

### Server-Side Optimization

#### Thread Configuration
Adjust the number of threads based on your CPU cores:
```cpp
// In main.cpp
unsigned int num_threads = std::thread::hardware_concurrency();
if (num_threads == 0) {
    num_threads = 4; // Default to 4 threads
}
```

#### Persistence Tuning
Adjust persistence intervals based on data importance:
- **High importance**: 30 seconds
- **Medium importance**: 60 seconds
- **Low importance**: 300 seconds

#### Memory Management
Monitor memory usage and adjust limits:
```ini
# Memory settings
max_memory=4GB
eviction_policy=allkeys-lru
```

### Client-Side Optimization

#### Connection Pooling
Use connection pools for high-concurrency applications:
```java
// Create a pool with appropriate size
ConnectionPool pool = new ConnectionPool("localhost", 7379, 50);
```

#### Asynchronous Operations
Use asynchronous operations to avoid blocking:
```java
// Instead of blocking operations
CompletableFuture<String> future = client.getAsync("key");
future.thenAccept(value -> {
    // Process value
});
```

#### Batch Operations
When possible, batch multiple operations:
```java
// Instead of multiple individual operations
List<CompletableFuture<Boolean>> futures = new ArrayList<>();
for (int i = 0; i < 100; i++) {
    futures.add(client.setAsync("key" + i, "value" + i));
}

// Wait for all operations to complete
CompletableFuture.allOf(futures.toArray(new CompletableFuture[0])).join();
```

## Monitoring and Maintenance

### Health Checks

#### Server Health
Monitor server health through:
1. **PING commands**: Regular PING to check responsiveness
2. **Connection counts**: Monitor active connections
3. **Memory usage**: Track memory consumption
4. **CPU usage**: Monitor CPU utilization

#### Client Health
Monitor client health through:
1. **Connection status**: Check if connections are active
2. **Response times**: Measure operation latency
3. **Error rates**: Track failed operations

### Maintenance Tasks

#### Data Backup
Regularly backup persistence files:
```bash
# Copy RDB file to backup location
cp redicraft.rdb /backup/location/redicraft_$(date +%Y%m%d_%H%M%S).rdb
```

#### Log Analysis
Analyze server logs for:
1. **Error patterns**: Identify recurring issues
2. **Performance bottlenecks**: Find slow operations
3. **Connection issues**: Track connection problems

#### Performance Monitoring
Monitor key metrics:
1. **Requests per second**: Track throughput
2. **Average response time**: Monitor latency
3. **Memory usage**: Track resource consumption
4. **Connection pool utilization**: Monitor pool efficiency

### Troubleshooting Common Issues

#### Connection Problems
1. **Check server status**: Ensure server is running
2. **Verify network connectivity**: Test network connection
3. **Check firewall settings**: Ensure ports are open
4. **Review configuration**: Verify host/port settings

#### Performance Issues
1. **Monitor resource usage**: Check CPU, memory, disk I/O
2. **Review thread configuration**: Adjust thread count
3. **Optimize persistence settings**: Tune save intervals
4. **Use connection pooling**: Implement connection reuse

#### Data Consistency
1. **Check replication status**: Verify master-slave sync
2. **Review cluster health**: Ensure all nodes are healthy
3. **Monitor persistence**: Verify data is being saved
4. **Check expiration settings**: Ensure TTL is working correctly

### Best Practices

#### Development
1. **Always close connections**: Prevent resource leaks
2. **Handle exceptions**: Implement proper error handling
3. **Use connection pooling**: Improve performance
4. **Test thoroughly**: Verify all operations work correctly

#### Production
1. **Monitor continuously**: Implement monitoring solutions
2. **Backup regularly**: Schedule regular data backups
3. **Scale appropriately**: Adjust resources based on load
4. **Update regularly**: Keep software up to date

#### Security
1. **Use firewalls**: Restrict access to necessary IPs only
2. **Monitor access**: Log all connection attempts
3. **Plan for encryption**: Future-proof for secure connections
4. **Implement authentication**: Add user authentication when available