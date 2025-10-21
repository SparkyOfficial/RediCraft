/*
 * ConnectionPool.java
 * author: Андрій Будильников
 */

package com.redicraft;

import java.io.IOException;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
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
    private final Semaphore connectionSemaphore;
    
    public ConnectionPool(String host, int port, int maxConnections) {
        this.host = host;
        this.port = port;
        this.maxConnections = maxConnections;
        this.availableConnections = new ConcurrentLinkedQueue<>();
        this.currentConnections = new AtomicInteger(0);
        this.connectionSemaphore = new Semaphore(maxConnections);
    }
    
    /**
     * Get a connection from the pool
     * @return a RedicraftClient connection
     * @throws IOException if unable to create a new connection
     * @throws InterruptedException if the thread is interrupted while waiting
     */
    public RedicraftClient getConnection() throws IOException, InterruptedException {
        // Try to acquire a permit for a connection
        // This will block until a connection is available or timeout occurs
        if (!connectionSemaphore.tryAcquire(5, TimeUnit.SECONDS)) {
            throw new IOException("Timeout waiting for a connection from the pool");
        }
        
        // Try to get an existing connection from the pool
        RedicraftClient client = availableConnections.poll();
        
        if (client != null) {
            // We found an available connection, return it
            return client;
        }
        
        // No available connections, create a new one
        client = new RedicraftClient();
        client.connect(host, port);
        currentConnections.incrementAndGet();
        return client;
    }
    
    /**
     * Return a connection to the pool
     * @param client the connection to return
     */
    public void returnConnection(RedicraftClient client) {
        if (client != null) {
            availableConnections.offer(client);
            // Release the permit so other threads can acquire connections
            connectionSemaphore.release();
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
        // Reset the semaphore to allow new connections
        connectionSemaphore.release(maxConnections - connectionSemaphore.availablePermits());
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