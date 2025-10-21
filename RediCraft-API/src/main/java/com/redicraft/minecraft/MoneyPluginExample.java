/*
 * MoneyPluginExample.java
 * author: Андрій Будильников
 */

package com.redicraft.minecraft;

import java.io.IOException;

import com.redicraft.RedicraftClient;

/**
 * Example Minecraft plugin showing how to use RediCraft client
 * This is a simplified example - a real Minecraft plugin would need to extend
 * the appropriate plugin class and implement the Bukkit/Spigot API
 */
public class MoneyPluginExample {
    private RedicraftClient client;
    
    public MoneyPluginExample() {
        client = new RedicraftClient();
        try {
            client.connect("localhost", 7379);
        } catch (IOException e) {
            System.err.println("Failed to connect to RediCraft server: " + e.getMessage());
        }
    }
    
    /**
     * Get a player's money balance
     * @param playerName the player's name
     * @return the player's money balance
     */
    public String getPlayerMoney(String playerName) {
        try {
            return client.get("player:" + playerName + ":money");
        } catch (IOException e) {
            System.err.println("Failed to get player money: " + e.getMessage());
            return null;
        }
    }
    
    /**
     * Set a player's money balance
     * @param playerName the player's name
     * @param amount the amount to set
     */
    public void setPlayerMoney(String playerName, String amount) {
        try {
            client.set("player:" + playerName + ":money", amount);
        } catch (IOException e) {
            System.err.println("Failed to set player money: " + e.getMessage());
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