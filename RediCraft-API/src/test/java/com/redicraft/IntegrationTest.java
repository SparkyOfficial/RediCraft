/*
 * IntegrationTest.java
 * author: Андрій Будильников
 */

package com.redicraft;

import java.io.IOException;

/**
 * Simple integration test to verify the server and client work together
 * This test requires the RediCraft server to be running on localhost:7379
 */
public class IntegrationTest {
    
    public static void main(String[] args) {
        RedicraftClient client = new RedicraftClient();
        
        try {
            // Connect to the server
            System.out.println("Connecting to RediCraft server...");
            client.connect("localhost", 7379);
            System.out.println("Connected successfully!");
            
            // Test PING
            System.out.println("\nTesting PING command...");
            boolean pong = client.ping();
            System.out.println("PING response: " + (pong ? "PONG" : "ERROR"));
            
            // Test SET/GET
            System.out.println("\nTesting SET/GET commands...");
            client.set("test_key", "test_value");
            String value = client.get("test_key");
            System.out.println("SET test_key=test_value, GET test_key=" + value);
            
            // Test INCR
            System.out.println("\nTesting INCR command...");
            long counter1 = client.incr("counter");
            System.out.println("INCR counter = " + counter1);
            long counter2 = client.incr("counter");
            System.out.println("INCR counter = " + counter2);
            
            // Test DECR
            System.out.println("\nTesting DECR command...");
            long counter3 = client.decr("counter");
            System.out.println("DECR counter = " + counter3);
            
            // Test INCRBY
            System.out.println("\nTesting INCRBY command...");
            long counter4 = client.incrBy("counter", 5);
            System.out.println("INCRBY counter 5 = " + counter4);
            long counter5 = client.incrBy("counter", -3);
            System.out.println("INCRBY counter -3 = " + counter5);
            
            // Close connection
            client.close();
            System.out.println("\nConnection closed.");
            
        } catch (IOException e) {
            System.err.println("Error: " + e.getMessage());
            e.printStackTrace();
        }
    }
}