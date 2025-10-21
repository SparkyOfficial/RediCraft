/*
 * ConnectionPool.java
 * author: Андрій Будильников
 */

package com.redicraft;

import java.io.IOException;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Simple connection pool for RediCraft clients
 */
public class ConnectionPool {
    private final String host;
    private final int port;
    private final int maxConnections;
    private final ConcurrentLinkedQueue<RedicraftClient> availableConnections;
    private final AtomicInteger currentConnections;
    
    public ConnectionPool(String host, int port, int maxConnections) {
        this.host = host;
        this.port = port;
        this.maxConnections = maxConnections;
        this.availableConnections = new ConcurrentLinkedQueue<>();
        this.currentConnections = new AtomicInteger(0);
    }
    
    /**
     * Get a connection from the pool
     * @return a RedicraftClient connection
     * @throws IOException if unable to create a new connection
     */
    public RedicraftClient getConnection() throws IOException {
        // Try to get an existing connection from the pool
        RedicraftClient client = availableConnections.poll();
        
        if (client != null) {
            // We found an available connection, return it
            return client;
        }
        
        // No available connections, check if we can create a new one
        if (currentConnections.get() < maxConnections) {
            // Create a new connection
            client = new RedicraftClient();
            client.connect(host, port);
            currentConnections.incrementAndGet();
            return client;
        }
        
        // We've reached the maximum number of connections
        // This implementation blocks until a connection is available
        // In a more sophisticated implementation, we might want to have a timeout
        while ((client = availableConnections.poll()) == null) {
            try {
                Thread.sleep(10); // Short sleep to avoid busy waiting
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                throw new IOException("Interrupted while waiting for a connection");
            }
        }
        
        return client;
    }
    
    /**
     * Return a connection to the pool
     * @param client the connection to return
     */
    public void returnConnection(RedicraftClient client) {
        if (client != null) {
            availableConnections.offer(client);
        }
    }
    
    /**
     * Close all connections in the pool
     */
    public void closeAllConnections() {
        RedicraftClient client;
        while ((client = availableConnections.poll()) != null) {
            try {
                client.close();
            } catch (IOException e) {
                // Ignore errors when closing connections during shutdown
            }
        }
        currentConnections.set(0);
    }
    
    /**
     * Get the number of currently active connections
     * @return the number of active connections
     */
    public int getActiveConnections() {
        return currentConnections.get();
    }
    
    /**
     * Get the number of available connections in the pool
     * @return the number of available connections
     */
    public int getAvailableConnections() {
        return availableConnections.size();
    }
}