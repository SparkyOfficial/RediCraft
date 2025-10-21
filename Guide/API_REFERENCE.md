# RediCraft API Reference

## Table of Contents
1. [RedicraftClient](#redicraftclient)
2. [ConnectionPool](#connectionpool)
3. [Command Reference](#command-reference)
4. [Error Handling](#error-handling)

## RedicraftClient

The `RedicraftClient` class is the main interface for interacting with the RediCraft server.

### Constructor
```java
public RedicraftClient()
```
Creates a new RedicraftClient instance.

### Methods

#### connect
```java
public boolean connect(String host, int port)
```
Establishes a connection to the RediCraft server.

**Parameters:**
- `host`: Server hostname or IP address
- `port`: Server port number

**Returns:** `true` if connection successful, `false` otherwise

#### close
```java
public void close()
```
Closes the connection to the server.

#### ping
```java
public boolean ping()
```
Sends a PING command to the server.

**Returns:** `true` if PONG received, `false` otherwise

#### set
```java
public boolean set(String key, String value)
```
Sets a key-value pair in the server.

**Parameters:**
- `key`: The key to set
- `value`: The value to associate with the key

**Returns:** `true` if successful, `false` otherwise

#### get
```java
public String get(String key)
```
Retrieves the value associated with a key.

**Parameters:**
- `key`: The key to retrieve

**Returns:** The value associated with the key, or `null` if key doesn't exist

#### incr
```java
public long incr(String key)
```
Increments the integer value of a key by 1.

**Parameters:**
- `key`: The key to increment

**Returns:** The new value after incrementing

#### decr
```java
public long decr(String key)
```
Decrements the integer value of a key by 1.

**Parameters:**
- `key`: The key to decrement

**Returns:** The new value after decrementing

#### incrBy
```java
public long incrBy(String key, long increment)
```
Increments the integer value of a key by the given amount.

**Parameters:**
- `key`: The key to increment
- `increment`: The amount to increment by

**Returns:** The new value after incrementing

#### hset
```java
public boolean hset(String key, String field, String value)
```
Sets a field in a hash stored at key.

**Parameters:**
- `key`: The key of the hash
- `field`: The field to set
- `value`: The value to set

**Returns:** `true` if field is new, `false` if field was updated

#### hget
```java
public String hget(String key, String field)
```
Gets the value of a field in a hash stored at key.

**Parameters:**
- `key`: The key of the hash
- `field`: The field to get

**Returns:** The value of the field, or `null` if field doesn't exist

#### hgetAll
```java
public Map<String, String> hgetAll(String key)
```
Gets all fields and values in a hash stored at key.

**Parameters:**
- `key`: The key of the hash

**Returns:** A map of all fields and values, or empty map if key doesn't exist

#### lpush
```java
public long lpush(String key, String... values)
```
Pushes values to the head of a list stored at key.

**Parameters:**
- `key`: The key of the list
- `values`: The values to push

**Returns:** The length of the list after the push operation

#### rpop
```java
public String rpop(String key)
```
Removes and returns the last element of a list stored at key.

**Parameters:**
- `key`: The key of the list

**Returns:** The value of the popped element, or `null` if list is empty

#### lrange
``java
public List<String> lrange(String key, long start, long end)
```
Gets a range of elements from a list stored at key.

**Parameters:**
- `key`: The key of the list
- `start`: Start index
- `end`: End index

**Returns:** A list of elements in the specified range

#### expire
```java
public boolean expire(String key, int seconds)
```
Sets a timeout on a key.

**Parameters:**
- `key`: The key to set timeout on
- `seconds`: Timeout in seconds

**Returns:** `true` if timeout was set, `false` if key doesn't exist

#### ttl
```java
public long ttl(String key)
```
Gets the time to live for a key.

**Parameters:**
- `key`: The key to check

**Returns:** 
- TTL in seconds if key exists and has timeout
- `-1` if key exists but has no timeout
- `-2` if key doesn't exist

### Asynchronous Methods

#### setAsync
```java
public CompletableFuture<Boolean> setAsync(String key, String value)
```
Asynchronously sets a key-value pair.

**Parameters:**
- `key`: The key to set
- `value`: The value to associate with the key

**Returns:** CompletableFuture that completes with the result

#### getAsync
```java
public CompletableFuture<String> getAsync(String key)
```
Asynchronously retrieves the value associated with a key.

**Parameters:**
- `key`: The key to retrieve

**Returns:** CompletableFuture that completes with the value

#### Other Async Methods
All synchronous methods have corresponding async versions:
- `incrAsync(key)`
- `decrAsync(key)`
- `incrByAsync(key, increment)`
- `hsetAsync(key, field, value)`
- `hgetAsync(key, field)`
- `hgetAllAsync(key)`
- `lpushAsync(key, values...)`
- `rpopAsync(key)`
- `lrangeAsync(key, start, end)`
- `expireAsync(key, seconds)`
- `ttlAsync(key)`

## ConnectionPool

The `ConnectionPool` class manages a pool of RedicraftClient connections for improved performance.

### Constructor
```java
public ConnectionPool(String host, int port, int maxConnections)
```
Creates a new connection pool.

**Parameters:**
- `host`: Server hostname or IP address
- `port`: Server port number
- `maxConnections`: Maximum number of connections in the pool

### Methods

#### getConnection
```java
public RedicraftClient getConnection()
```
Gets a connection from the pool.

**Returns:** A RedicraftClient instance from the pool

#### returnConnection
```java
public void returnConnection(RedicraftClient client)
```
Returns a connection to the pool.

**Parameters:**
- `client`: The client to return to the pool

#### close
```java
public void close()
```
Closes all connections in the pool.

## Command Reference

### String Commands

#### SET
```
SET key value
```
Set key to hold the string value.

**Return:** "OK"

#### GET
```
GET key
```
Get the value of key.

**Return:** The value of key, or (nil) when key does not exist.

#### INCR
```
INCR key
```
Increments the number stored at key by one.

**Return:** The value of key after the increment.

#### DECR
```
DECR key
```
Decrements the number stored at key by one.

**Return:** The value of key after the decrement.

#### INCRBY
```
INCRBY key increment
```
Increments the number stored at key by increment.

**Return:** The value of key after the increment.

### Hash Commands

#### HSET
```
HSET key field value
```
Sets field in the hash stored at key to value.

**Return:** 1 if field is a new field, 0 if field was updated.

#### HGET
```
HGET key field
```
Returns the value associated with field in the hash stored at key.

**Return:** The value associated with field, or (nil) when field is not present.

#### HGETALL
```
HGETALL key
```
Returns all fields and values of the hash stored at key.

**Return:** List of fields and their values.

### List Commands

#### LPUSH
```
LPUSH key element [element ...]
```
Insert all the specified values at the head of the list stored at key.

**Return:** The length of the list after the push operations.

#### RPOP
```
RPOP key
```
Removes and returns the last element of the list stored at key.

**Return:** The value of the last element, or (nil) when key does not exist.

#### LRANGE
```
LRANGE key start stop
```
Returns the specified elements of the list stored at key.

**Return:** List of elements in the specified range.

### Set Commands

#### SADD
```
SADD key member [member ...]
```
Add the specified members to the set stored at key. Specified members that are already a member of this set are ignored. If key does not exist, a new set is created before adding the specified members.

**Return:** The number of elements that were added to the set, not including all the elements already present in the set.

#### SMEMBERS
```
SMEMBERS key
```
Returns all the members of the set value stored at key.

**Return:** All members of the set.

#### SREM
```
SREM key member [member ...]
```
Remove the specified members from the set stored at key. Specified members that are not a member of this set are ignored. If key does not exist, it is treated as an empty set and this command returns 0.

**Return:** The number of members that were removed from the set, not including non existing members.

#### SISMEMBER
```
SISMEMBER key member
```
Returns if member is a member of the set stored at key.

**Return:** 1 if the element is a member of the set, 0 if the element is not a member of the set, or if key does not exist.

#### SCARD
```
SCARD key
```
Returns the set cardinality (number of elements) of the set stored at key.

**Return:** The cardinality (number of elements) of the set, or 0 if key does not exist.

### Expiration Commands

#### EXPIRE
```
EXPIRE key seconds
```
Set a timeout on key.

**Return:** 1 if the timeout was set, 0 if key does not exist.

#### TTL
```
TTL key
```
Returns the remaining time to live of a key that has a timeout.

**Return:** TTL in seconds, or -1 when key exists but has no timeout, or -2 when key does not exist.

## Error Handling

### Exception Types

#### RedicraftException
Base exception class for all RediCraft-related exceptions.

#### ConnectionException
Thrown when there are connection-related issues.

#### CommandException
Thrown when there are command execution errors.

### Best Practices

1. **Always close connections** when done:
```
RedicraftClient client = new RedicraftClient();
try {
    client.connect("localhost", 7379);
    // ... use client
} finally {
    client.close();
}
```

2. **Use connection pooling** for high-concurrency scenarios:
```
ConnectionPool pool = new ConnectionPool("localhost", 7379, 10);
try {
    RedicraftClient client = pool.getConnection();
    // ... use client
    pool.returnConnection(client);
} finally {
    pool.close();
}
```

3. **Handle exceptions appropriately**:
```java
try {
    client.set("key", "value");
} catch (RedicraftException e) {
    // Handle RediCraft-specific exceptions
    logger.error("RediCraft error: " + e.getMessage());
} catch (Exception e) {
    // Handle other exceptions
    logger.error("Unexpected error: " + e.getMessage());
}
```

4. **Use asynchronous operations** for non-blocking calls:
```java
CompletableFuture<Boolean> future = client.setAsync("key", "value");
future.thenAccept(result -> {
    System.out.println("Set operation completed: " + result);
}).exceptionally(throwable -> {
    System.err.println("Set operation failed: " + throwable.getMessage());
    return null;
});
```