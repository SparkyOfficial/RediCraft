/*
 * AsyncMoneyPluginExample.java
 * author: Андрій Будильников
 */

package com.redicraft.minecraft;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;

import com.redicraft.ConnectionPool;
import com.redicraft.RedicraftClient;

/**
 * Example showing how to implement money commands using RediCraft with async operations and connection pooling
 * This is a simplified example - a real Minecraft plugin would need to extend
 * the appropriate plugin class and implement the Bukkit/Spigot API
 */
public class AsyncMoneyPluginExample {
    private ConnectionPool connectionPool;
    
    public AsyncMoneyPluginExample() {
        // Create a connection pool with a maximum of 10 connections
        connectionPool = new ConnectionPool("localhost", 7379, 10);
    }
    
    /**
     * Handle the /money command asynchronously
     * @param playerName the player's name
     * @return CompletableFuture that will complete with the player's money balance
     */
    public CompletableFuture<String> handleMoneyCommandAsync(String playerName) {
        return CompletableFuture.supplyAsync(() -> {
            RedicraftClient client = null;
            try {
                client = connectionPool.getConnection();
                String balance = client.get("player:" + playerName + ":money");
                if (balance == null) {
                    // Player has no money record, return 0
                    return "0";
                }
                return balance;
            } catch (IOException e) {
                System.err.println("Failed to get player money: " + e.getMessage());
                return "Error retrieving balance";
            } finally {
                if (client != null) {
                    connectionPool.returnConnection(client);
                }
            }
        });
    }
    
    /**
     * Handle the /setmoney command asynchronously
     * @param playerName the player's name
     * @param amount the amount to set
     * @return CompletableFuture that will complete with a success message
     */
    public CompletableFuture<String> handleSetMoneyCommandAsync(String playerName, String amount) {
        return CompletableFuture.supplyAsync(() -> {
            RedicraftClient client = null;
            try {
                client = connectionPool.getConnection();
                client.set("player:" + playerName + ":money", amount);
                return "Set " + playerName + "'s balance to " + amount;
            } catch (IOException e) {
                System.err.println("Failed to set player money: " + e.getMessage());
                return "Error setting balance";
            } finally {
                if (client != null) {
                    connectionPool.returnConnection(client);
                }
            }
        });
    }
    
    /**
     * Handle the /pay command asynchronously
     * @param fromPlayer the player sending money
     * @param toPlayer the player receiving money
     * @param amount the amount to transfer
     * @return CompletableFuture that will complete with a success message
     */
    public CompletableFuture<String> handlePayCommandAsync(String fromPlayer, String toPlayer, String amount) {
        return CompletableFuture.supplyAsync(() -> {
            RedicraftClient client = null;
            try {
                client = connectionPool.getConnection();
                
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
            } finally {
                if (client != null) {
                    connectionPool.returnConnection(client);
                }
            }
        });
    }
    
    /**
     * Handle multiple operations concurrently
     * @param playerNames array of player names
     * @return CompletableFuture that will complete with the total balance of all players
     */
    public CompletableFuture<Long> getTotalBalanceAsync(String[] playerNames) {
        // Create an array of CompletableFuture for each player's balance
        CompletableFuture<Long>[] futures = new CompletableFuture[playerNames.length];
        
        for (int i = 0; i < playerNames.length; i++) {
            final String playerName = playerNames[i];
            futures[i] = CompletableFuture.supplyAsync(() -> {
                RedicraftClient client = null;
                try {
                    client = connectionPool.getConnection();
                    String balanceStr = client.get("player:" + playerName + ":money");
                    return balanceStr == null ? 0L : Long.parseLong(balanceStr);
                } catch (IOException e) {
                    System.err.println("Failed to get balance for " + playerName + ": " + e.getMessage());
                    return 0L;
                } catch (NumberFormatException e) {
                    return 0L;
                } finally {
                    if (client != null) {
                        connectionPool.returnConnection(client);
                    }
                }
            });
        }
        
        // Combine all futures into one that completes when all are done
        return CompletableFuture.allOf(futures)
            .thenApply(v -> {
                long total = 0;
                for (CompletableFuture<Long> future : futures) {
                    total += future.join();
                }
                return total;
            });
    }
    
    /**
     * Close the connection pool
     */
    public void close() {
        if (connectionPool != null) {
            connectionPool.closeAllConnections();
        }
    }
}