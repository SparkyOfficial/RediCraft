/*
 * MoneyCommandExample.java
 * author: Андрій Будильников
 */

package com.redicraft.minecraft;

import java.io.IOException;
import java.util.Map;

import com.redicraft.RedicraftClient;

/**
 * Example showing how to implement money commands using RediCraft
 * This is a simplified example - a real Minecraft plugin would need to extend
 * the appropriate plugin class and implement the Bukkit/Spigot API
 */
public class MoneyCommandExample {
    private RedicraftClient client;
    
    public MoneyCommandExample() {
        client = new RedicraftClient();
        try {
            // Connect to the RediCraft server
            client.connect("localhost", 7379);
        } catch (IOException e) {
            System.err.println("Failed to connect to RediCraft server: " + e.getMessage());
        }
    }
    
    /**
     * Handle the /money command
     * @param playerName the player's name
     * @return the player's money balance
     */
    public String handleMoneyCommand(String playerName) {
        try {
            String balance = client.get("player:" + playerName + ":money");
            if (balance == null) {
                // Player has no money record, return 0
                return "0";
            }
            return balance;
        } catch (IOException e) {
            System.err.println("Failed to get player money: " + e.getMessage());
            return "Error retrieving balance";
        }
    }
    
    /**
     * Handle the /setmoney command
     * @param playerName the player's name
     * @param amount the amount to set
     * @return success message
     */
    public String handleSetMoneyCommand(String playerName, String amount) {
        try {
            client.set("player:" + playerName + ":money", amount);
            return "Set " + playerName + "'s balance to " + amount;
        } catch (IOException e) {
            System.err.println("Failed to set player money: " + e.getMessage());
            return "Error setting balance";
        }
    }
    
    /**
     * Handle the /pay command
     * @param fromPlayer the player sending money
     * @param toPlayer the player receiving money
     * @param amount the amount to transfer
     * @return success message
     */
    public String handlePayCommand(String fromPlayer, String toPlayer, String amount) {
        try {
            // Get sender's balance
            String fromBalanceStr = client.get("player:" + fromPlayer + ":money");
            long fromBalance = fromBalanceStr == null ? 0 : Long.parseLong(fromBalanceStr);
            
            // Get recipient's balance
            String toBalanceStr = client.get("player:" + toPlayer + ":money");
            long toBalance = toBalanceStr == null ? 0 : Long.parseLong(toBalanceStr);
            
            // Parse amount to transfer
            long transferAmount = Long.parseLong(amount);
            
            // Check if sender has enough money
            if (fromBalance < transferAmount) {
                return "Insufficient funds";
            }
            
            // Perform the transfer using INCRBY for atomic operations
            client.incrBy("player:" + fromPlayer + ":money", -transferAmount);
            client.incrBy("player:" + toPlayer + ":money", transferAmount);
            
            return "Transferred " + transferAmount + " to " + toPlayer;
        } catch (IOException e) {
            System.err.println("Failed to process payment: " + e.getMessage());
            return "Error processing payment";
        } catch (NumberFormatException e) {
            return "Invalid amount";
        }
    }
    
    /**
     * Handle the /givemoney command
     * @param playerName the player's name
     * @param amount the amount to give
     * @return success message
     */
    public String handleGiveMoneyCommand(String playerName, String amount) {
        try {
            long givenAmount = Long.parseLong(amount);
            long newBalance = client.incrBy("player:" + playerName + ":money", givenAmount);
            return "Gave " + amount + " to " + playerName + ". New balance: " + newBalance;
        } catch (IOException e) {
            System.err.println("Failed to give money: " + e.getMessage());
            return "Error giving money";
        } catch (NumberFormatException e) {
            return "Invalid amount";
        }
    }
    
    /**
     * Handle the /takemoney command
     * @param playerName the player's name
     * @param amount the amount to take
     * @return success message
     */
    public String handleTakeMoneyCommand(String playerName, String amount) {
        try {
            long takenAmount = Long.parseLong(amount);
            long newBalance = client.incrBy("player:" + playerName + ":money", -takenAmount);
            return "Took " + amount + " from " + playerName + ". New balance: " + newBalance;
        } catch (IOException e) {
            System.err.println("Failed to take money: " + e.getMessage());
            return "Error taking money";
        } catch (NumberFormatException e) {
            return "Invalid amount";
        }
    }
    
    /**
     * Handle the /playerinfo command - demonstrates hash operations
     * @param playerName the player's name
     * @return player information
     */
    public String handlePlayerInfoCommand(String playerName) {
        try {
            // Store player info in a hash
            client.hset("player:" + playerName + ":info", "name", playerName);
            client.hset("player:" + playerName + ":info", "level", "15");
            client.hset("player:" + playerName + ":info", "class", "Warrior");
            
            // Retrieve all player info
            Map<String, String> info = client.hgetAll("player:" + playerName + ":info");
            
            StringBuilder result = new StringBuilder("Player Info for " + playerName + ":\n");
            for (Map.Entry<String, String> entry : info.entrySet()) {
                result.append(entry.getKey()).append(": ").append(entry.getValue()).append("\n");
            }
            
            return result.toString();
        } catch (IOException e) {
            System.err.println("Failed to get player info: " + e.getMessage());
            return "Error retrieving player info";
        }
    }
    
    /**
     * Handle the /recentlogins command - demonstrates list operations
     * @param playerName the player's name
     * @return recent login messages
     */
    public String handleRecentLoginsCommand(String playerName) {
        try {
            // Add a login entry to the list
            client.lpush("recent_logins", playerName + " logged in at " + System.currentTimeMillis());
            
            // Keep only the last 10 logins
            // In a real implementation, you might want to trim the list periodically
            // For production use, you would implement a proper LTRIM command in the client
            // or use a combination of LLEN and LPOP/RPOP to maintain list size
            
            // Since we don't have lrange in the client, we'll return a simple message
            // In a real implementation, you would add lrange to the client or use other commands
            // to retrieve and display the recent logins
            return "Login recorded for " + playerName + " (list trimming would happen in production)";
        } catch (IOException e) {
            System.err.println("Failed to handle recent logins: " + e.getMessage());
            return "Error handling recent logins";
        }
    }
    
    /**
     * Handle the /tempban command - demonstrates expiration
     * @param playerName the player's name
     * @param duration the ban duration in seconds
     * @return success message
     */
    public String handleTempBanCommand(String playerName, String duration) {
        try {
            long seconds = Long.parseLong(duration);
            
            // Set a temporary ban flag
            client.set("ban:" + playerName, "true");
            
            // Set expiration
            if (client.expire("ban:" + playerName, seconds)) {
                return "Player " + playerName + " banned for " + seconds + " seconds";
            } else {
                return "Failed to set ban expiration";
            }
        } catch (IOException e) {
            System.err.println("Failed to temp ban player: " + e.getMessage());
            return "Error banning player";
        } catch (NumberFormatException e) {
            return "Invalid duration";
        }
    }
    
    /**
     * Check if a player is banned
     * @param playerName the player's name
     * @return true if banned, false otherwise
     */
    public boolean isPlayerBanned(String playerName) {
        try {
            // Check if ban key exists and hasn't expired
            long ttl = client.ttl("ban:" + playerName);
            return ttl >= 0; // Key exists and hasn't expired
        } catch (IOException e) {
            System.err.println("Failed to check ban status: " + e.getMessage());
            return false;
        }
    }
    
    /**
     * Close the connection to the RediCraft server
     */
    public void close() {
        try {
            client.close();
        } catch (IOException e) {
            System.err.println("Failed to close connection: " + e.getMessage());
        }
    }
}
