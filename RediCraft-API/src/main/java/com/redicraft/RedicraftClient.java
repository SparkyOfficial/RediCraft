/*
 * RedicraftClient.java
 * author: Андрій Будильников
 */

package com.redicraft;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Client for connecting to the RediCraft key-value server
 */
public class RedicraftClient {
    private static final String NIL_RESPONSE = "(nil)";
    private static final String EMPTY_HASH_RESPONSE = "(empty hash)";
    private static final String EMPTY_SET_RESPONSE = "(empty set)";
    
    private Socket socket;
    private PrintWriter out;
    private BufferedReader in;
    private static final ExecutorService SHARED_EXECUTOR = Executors.newCachedThreadPool();
    
    public RedicraftClient() {
        // No longer create a single-thread executor for each client
        // Use the shared executor service instead
    }
    
    /**
     * Connect to the RediCraft server
     * @param host server hostname
     * @param port server port
     * @throws IOException if connection fails
     */
    public void connect(String host, int port) throws IOException {
        socket = new Socket(host, port);
        out = new PrintWriter(socket.getOutputStream(), true);
        in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
    }
    
    /**
     * Send PING command to server
     * @return true if server responds with PONG
     * @throws IOException if communication fails
     */
    public boolean ping() throws IOException {
        out.println("PING");
        String response = in.readLine();
        return "PONG".equals(response);
    }
    
    /**
     * Send PING command to server asynchronously
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<Boolean> pingAsync() {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return ping();
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Set a key-value pair in the server
     * @param key the key
     * @param value the value
     * @throws IOException if communication fails
     */
    public void set(String key, String value) throws IOException {
        out.println("SET " + key + " " + value);
        // Read the response
        String response = in.readLine();
        // Fix for SonarLint issue: Use or store the value returned from "readLine"
        if (response == null) {
            throw new IOException("No response from server");
        }
    }
    
    /**
     * Set a key-value pair in the server asynchronously
     * @param key the key
     * @param value the value
     * @return CompletableFuture that will complete when the operation is done
     */
    public CompletableFuture<Void> setAsync(String key, String value) {
        return CompletableFuture.runAsync(() -> {
            try {
                set(key, value);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Get the value for a key from the server
     * @param key the key
     * @return the value, or null if not found
     * @throws IOException if communication fails
     */
    public String get(String key) throws IOException {
        out.println("GET " + key);
        String response = in.readLine();
        return NIL_RESPONSE.equals(response) ? null : response;
    }
    
    /**
     * Get the value for a key from the server asynchronously
     * @param key the key
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<String> getAsync(String key) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return get(key);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Increment the value of a key by 1
     * @param key the key
     * @return the new value
     * @throws IOException if communication fails
     */
    public long incr(String key) throws IOException {
        out.println("INCR " + key);
        String response = in.readLine();
        try {
            return Long.parseLong(response);
        } catch (NumberFormatException e) {
            throw new IOException("Invalid response from server: " + response);
        }
    }
    
    /**
     * Increment the value of a key by 1 asynchronously
     * @param key the key
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<Long> incrAsync(String key) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return incr(key);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Decrement the value of a key by 1
     * @param key the key
     * @return the new value
     * @throws IOException if communication fails
     */
    public long decr(String key) throws IOException {
        out.println("DECR " + key);
        String response = in.readLine();
        try {
            return Long.parseLong(response);
        } catch (NumberFormatException e) {
            throw new IOException("Invalid response from server: " + response);
        }
    }
    
    /**
     * Decrement the value of a key by 1 asynchronously
     * @param key the key
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<Long> decrAsync(String key) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return decr(key);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Increment the value of a key by the given amount
     * @param key the key
     * @param increment the amount to increment by
     * @return the new value
     * @throws IOException if communication fails
     */
    public long incrBy(String key, long increment) throws IOException {
        out.println("INCRBY " + key + " " + increment);
        String response = in.readLine();
        try {
            return Long.parseLong(response);
        } catch (NumberFormatException e) {
            throw new IOException("Invalid response from server: " + response);
        }
    }
    
    /**
     * Increment the value of a key by the given amount asynchronously
     * @param key the key
     * @param increment the amount to increment by
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<Long> incrByAsync(String key, long increment) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return incrBy(key, increment);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Set a field in a hash to a value
     * @param key the hash key
     * @param field the field name
     * @param value the value
     * @throws IOException if communication fails
     */
    public void hset(String key, String field, String value) throws IOException {
        out.println("HSET " + key + " " + field + " " + value);
        // Read the response
        String response = in.readLine();
        // Fix for SonarLint issue: Use or store the value returned from "readLine"
        if (response == null) {
            throw new IOException("No response from server");
        }
    }
    
    /**
     * Set a field in a hash to a value asynchronously
     * @param key the hash key
     * @param field the field name
     * @param value the value
     * @return CompletableFuture that will complete when the operation is done
     */
    public CompletableFuture<Void> hsetAsync(String key, String field, String value) {
        return CompletableFuture.runAsync(() -> {
            try {
                hset(key, field, value);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Get the value of a field in a hash
     * @param key the hash key
     * @param field the field name
     * @return the value, or null if not found
     * @throws IOException if communication fails
     */
    public String hget(String key, String field) throws IOException {
        out.println("HGET " + key + " " + field);
        String response = in.readLine();
        return NIL_RESPONSE.equals(response) ? null : response;
    }
    
    /**
     * Get the value of a field in a hash asynchronously
     * @param key the hash key
     * @param field the field name
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<String> hgetAsync(String key, String field) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return hget(key, field);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Get all fields and values in a hash
     * @param key the hash key
     * @return a map of field-value pairs
     * @throws IOException if communication fails
     */
    public Map<String, String> hgetAll(String key) throws IOException {
        out.println("HGETALL " + key);
        Map<String, String> result = new HashMap<>();
        
        String response;
        while ((response = in.readLine()) != null && !response.isEmpty()) {
            if (response.equals(EMPTY_HASH_RESPONSE)) {
                break;
            }
            // Parse field: value format
            int colonIndex = response.indexOf(": ");
            if (colonIndex > 0) {
                String field = response.substring(0, colonIndex);
                String value = response.substring(colonIndex + 2);
                result.put(field, value);
            }
        }
        
        return result;
    }
    
    /**
     * Get all fields and values in a hash asynchronously
     * @param key the hash key
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<Map<String, String>> hgetAllAsync(String key) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return hgetAll(key);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Add values to the head of a list
     * @param key the list key
     * @param values the values to add
     * @return the new length of the list
     * @throws IOException if communication fails
     */
    public long lpush(String key, String... values) throws IOException {
        StringBuilder command = new StringBuilder("LPUSH " + key);
        for (String value : values) {
            command.append(" ").append(value);
        }
        out.println(command.toString());
        
        String response = in.readLine();
        // Fix for SonarLint issue: Use or store the value returned from "readLine"
        if (response == null) {
            throw new IOException("No response from server");
        }
        
        try {
            return Long.parseLong(response);
        } catch (NumberFormatException e) {
            throw new IOException("Invalid response from server: " + response);
        }
    }
    
    /**
     * Add values to the head of a list asynchronously
     * @param key the list key
     * @param values the values to add
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<Long> lpushAsync(String key, String... values) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return lpush(key, values);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Remove and return the last element of a list
     * @param key the list key
     * @return the removed element, or null if list is empty
     * @throws IOException if communication fails
     */
    public String rpop(String key) throws IOException {
        out.println("RPOP " + key);
        String response = in.readLine();
        return NIL_RESPONSE.equals(response) ? null : response;
    }
    
    /**
     * Remove and return the last element of a list asynchronously
     * @param key the list key
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<String> rpopAsync(String key) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return rpop(key);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Add one or more members to a set
     * @param key the set key
     * @param members the members to add
     * @return the number of elements that were added to the set
     * @throws IOException if communication fails
     */
    public long sadd(String key, String... members) throws IOException {
        StringBuilder command = new StringBuilder("SADD " + key);
        for (String member : members) {
            command.append(" ").append(member);
        }
        out.println(command.toString());
        
        String response = in.readLine();
        // Fix for SonarLint issue: Use or store the value returned from "readLine"
        if (response == null) {
            throw new IOException("No response from server");
        }
        
        try {
            return Long.parseLong(response);
        } catch (NumberFormatException e) {
            throw new IOException("Invalid response from server: " + response);
        }
    }
    
    /**
     * Add one or more members to a set asynchronously
     * @param key the set key
     * @param members the members to add
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<Long> saddAsync(String key, String... members) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return sadd(key, members);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Remove one or more members from a set
     * @param key the set key
     * @param members the members to remove
     * @return the number of members that were removed from the set
     * @throws IOException if communication fails
     */
    public long srem(String key, String... members) throws IOException {
        StringBuilder command = new StringBuilder("SREM " + key);
        for (String member : members) {
            command.append(" ").append(member);
        }
        out.println(command.toString());
        
        String response = in.readLine();
        // Fix for SonarLint issue: Use or store the value returned from "readLine"
        if (response == null) {
            throw new IOException("No response from server");
        }
        
        try {
            return Long.parseLong(response);
        } catch (NumberFormatException e) {
            throw new IOException("Invalid response from server: " + response);
        }
    }
    
    /**
     * Remove one or more members from a set asynchronously
     * @param key the set key
     * @param members the members to remove
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<Long> sremAsync(String key, String... members) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return srem(key, members);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Get all members in a set
     * @param key the set key
     * @return a set of all members
     * @throws IOException if communication fails
     */
    public Set<String> smembers(String key) throws IOException {
        out.println("SMEMBERS " + key);
        Set<String> result = new HashSet<>();
        
        String response;
        while ((response = in.readLine()) != null && !response.isEmpty()) {
            if (response.equals(EMPTY_SET_RESPONSE)) {
                break;
            }
            result.add(response);
        }
        
        return result;
    }
    
    /**
     * Get all members in a set asynchronously
     * @param key the set key
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<Set<String>> smembersAsync(String key) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return smembers(key);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Determine if a given value is a member of a set
     * @param key the set key
     * @param member the member to check
     * @return true if the member exists in the set, false otherwise
     * @throws IOException if communication fails
     */
    public boolean sismember(String key, String member) throws IOException {
        out.println("SISMEMBER " + key + " " + member);
        String response = in.readLine();
        return "1".equals(response);
    }
    
    /**
     * Determine if a given value is a member of a set asynchronously
     * @param key the set key
     * @param member the member to check
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<Boolean> sismemberAsync(String key, String member) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return sismember(key, member);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Get the number of members in a set
     * @param key the set key
     * @return the cardinality (number of elements) of the set
     * @throws IOException if communication fails
     */
    public long scard(String key) throws IOException {
        out.println("SCARD " + key);
        String response = in.readLine();
        // Fix for SonarLint issue: Use or store the value returned from "readLine"
        if (response == null) {
            throw new IOException("No response from server");
        }
        
        try {
            return Long.parseLong(response);
        } catch (NumberFormatException e) {
            throw new IOException("Invalid response from server: " + response);
        }
    }
    
    /**
     * Get the number of members in a set asynchronously
     * @param key the set key
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<Long> scardAsync(String key) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return scard(key);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Set a timeout on a key
     * @param key the key
     * @param seconds the timeout in seconds
     * @return true if timeout was set, false otherwise
     * @throws IOException if communication fails
     */
    public boolean expire(String key, long seconds) throws IOException {
        out.println("EXPIRE " + key + " " + seconds);
        String response = in.readLine();
        return "1".equals(response);
    }
    
    /**
     * Set a timeout on a key asynchronously
     * @param key the key
     * @param seconds the timeout in seconds
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<Boolean> expireAsync(String key, long seconds) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return expire(key, seconds);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Get the time to live for a key
     * @param key the key
     * @return the TTL in seconds, -1 if key exists but no timeout, -2 if key doesn't exist
     * @throws IOException if communication fails
     */
    public long ttl(String key) throws IOException {
        out.println("TTL " + key);
        String response = in.readLine();
        // Fix for SonarLint issue: Use or store the value returned from "readLine"
        if (response == null) {
            throw new IOException("No response from server");
        }
        
        try {
            return Long.parseLong(response);
        } catch (NumberFormatException e) {
            throw new IOException("Invalid response from server: " + response);
        }
    }
    
    /**
     * Get the time to live for a key asynchronously
     * @param key the key
     * @return CompletableFuture that will complete with the result
     */
    public CompletableFuture<Long> ttlAsync(String key) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return ttl(key);
            } catch (IOException e) {
                throw new RedicraftException.Unchecked(e);
            }
        }, SHARED_EXECUTOR);
    }
    
    /**
     * Close the connection to the server
     * @throws IOException if closing fails
     */
    public void close() throws IOException {
        // Don't shutdown the shared executor as it's used by other clients
        if (in != null) {
            in.close();
        }
        if (out != null) {
            out.close();
        }
        if (socket != null) {
            socket.close();
        }
    }
}