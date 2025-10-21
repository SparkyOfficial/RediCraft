/*
 * TestSetCommands.java
 * author: Андрій Будильников
 */

package com.redicraft;

import java.io.IOException;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Simple test to verify set commands work correctly
 */
public class TestSetCommands {
    private static final Logger LOGGER = Logger.getLogger(TestSetCommands.class.getName());
    private static final String FRUITS_KEY = "fruits";
    private static final String APPLE = "apple";
    private static final String BANANA = "banana";
    private static final String ORANGE = "orange";
    private static final String GRAPE = "grape";
    private static final String KIWI = "kiwi";
    private static final String MANGO = "mango";
    
    public static void main(String[] args) {
        RedicraftClient client = new RedicraftClient();
        
        try {
            // Connect to the server
            LOGGER.info("Connecting to RediCraft server...");
            client.connect("localhost", 7379);
            LOGGER.info("Connected successfully!");
            
            // Test SADD command
            LOGGER.info("\nTesting SADD command...");
            long added = client.sadd(FRUITS_KEY, APPLE, BANANA, ORANGE);
            LOGGER.info("Added " + added + " fruits to the set");
            
            // Add more fruits (some duplicates)
            long added2 = client.sadd(FRUITS_KEY, APPLE, GRAPE, KIWI);
            LOGGER.info("Added " + added2 + " more fruits (apple was duplicate)");
            
            // Test SCARD command
            LOGGER.info("\nTesting SCARD command...");
            long count = client.scard(FRUITS_KEY);
            LOGGER.info("Number of fruits in set: " + count);
            
            // Test SMEMBERS command
            LOGGER.info("\nTesting SMEMBERS command...");
            Set<String> fruits = client.smembers(FRUITS_KEY);
            LOGGER.info("Fruits in set:");
            for (String fruit : fruits) {
                LOGGER.info("  - " + fruit);
            }
            
            // Test SISMEMBER command
            LOGGER.info("\nTesting SISMEMBER command...");
            boolean hasApple = client.sismember(FRUITS_KEY, APPLE);
            boolean hasMango = client.sismember(FRUITS_KEY, MANGO);
            LOGGER.info("Is 'apple' in fruits? " + (hasApple ? "Yes" : "No"));
            LOGGER.info("Is 'mango' in fruits? " + (hasMango ? "Yes" : "No"));
            
            // Test SREM command
            LOGGER.info("\nTesting SREM command...");
            long removed = client.srem(FRUITS_KEY, BANANA, MANGO);
            LOGGER.info("Removed " + removed + " fruits (mango wasn't in set)");
            
            // Check members after removal
            LOGGER.info("\nFruits after removal:");
            Set<String> remainingFruits = client.smembers(FRUITS_KEY);
            for (String fruit : remainingFruits) {
                LOGGER.info("  - " + fruit);
            }
            
            // Check final count
            long finalCount = client.scard(FRUITS_KEY);
            LOGGER.info("\nFinal count of fruits: " + finalCount);
            
            // Close connection
            client.close();
            LOGGER.info("\nConnection closed.");
            
        } catch (IOException e) {
            LOGGER.log(Level.SEVERE, "Error: " + e.getMessage(), e);
        }
    }
}