/*
 * TestSetCommands.java
 * author: Андрій Будильников
 */

package com.redicraft;

import java.io.IOException;
import java.util.Set;

/**
 * Simple test to verify set commands work correctly
 */
public class TestSetCommands {
    
    public static void main(String[] args) {
        RedicraftClient client = new RedicraftClient();
        
        try {
            // Connect to the server
            System.out.println("Connecting to RediCraft server...");
            client.connect("localhost", 7379);
            System.out.println("Connected successfully!");
            
            // Test SADD command
            System.out.println("\nTesting SADD command...");
            long added = client.sadd("fruits", "apple", "banana", "orange");
            System.out.println("Added " + added + " fruits to the set");
            
            // Add more fruits (some duplicates)
            long added2 = client.sadd("fruits", "apple", "grape", "kiwi");
            System.out.println("Added " + added2 + " more fruits (apple was duplicate)");
            
            // Test SCARD command
            System.out.println("\nTesting SCARD command...");
            long count = client.scard("fruits");
            System.out.println("Number of fruits in set: " + count);
            
            // Test SMEMBERS command
            System.out.println("\nTesting SMEMBERS command...");
            Set<String> fruits = client.smembers("fruits");
            System.out.println("Fruits in set:");
            for (String fruit : fruits) {
                System.out.println("  - " + fruit);
            }
            
            // Test SISMEMBER command
            System.out.println("\nTesting SISMEMBER command...");
            boolean hasApple = client.sismember("fruits", "apple");
            boolean hasMango = client.sismember("fruits", "mango");
            System.out.println("Is 'apple' in fruits? " + (hasApple ? "Yes" : "No"));
            System.out.println("Is 'mango' in fruits? " + (hasMango ? "Yes" : "No"));
            
            // Test SREM command
            System.out.println("\nTesting SREM command...");
            long removed = client.srem("fruits", "banana", "mango");
            System.out.println("Removed " + removed + " fruits (mango wasn't in set)");
            
            // Check members after removal
            System.out.println("\nFruits after removal:");
            Set<String> remainingFruits = client.smembers("fruits");
            for (String fruit : remainingFruits) {
                System.out.println("  - " + fruit);
            }
            
            // Check final count
            long finalCount = client.scard("fruits");
            System.out.println("\nFinal count of fruits: " + finalCount);
            
            // Close connection
            client.close();
            System.out.println("\nConnection closed.");
            
        } catch (IOException e) {
            System.err.println("Error: " + e.getMessage());
            e.printStackTrace();
        }
    }
}