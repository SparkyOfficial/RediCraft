/*
 * MoneyPluginExample.java
 * author: Андрій Будильников
 */

package com.redicraft.minecraft;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.redicraft.RedicraftClient;

/**
 * Example Minecraft plugin showing how to use RediCraft client
 * This is a simplified example - a real Minecraft plugin would need to extend
 * the appropriate plugin class and implement the Bukkit/Spigot API
 */
public class MoneyPluginExample {
    private static final Logger LOGGER = Logger.getLogger(MoneyPluginExample.class.getName());
    private RedicraftClient client;
    
    public MoneyPluginExample() {
        client = new RedicraftClient();
        try {
            client.connect("localhost", 7379);
        } catch (IOException e) {
            LOGGER.log(Level.SEVERE, "Failed to connect to RediCraft server: " + e.getMessage(), e);
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
            LOGGER.log(Level.SEVERE, "Failed to get player money: " + e.getMessage(), e);
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
            LOGGER.log(Level.SEVERE, "Failed to set player money: " + e.getMessage(), e);
        }
    }
    
    /**
     * Close the connection to the RediCraft server
     */
    public void close() {
        try {
            client.close();
        } catch (IOException e) {
            LOGGER.log(Level.SEVERE, "Failed to close connection: " + e.getMessage(), e);
        }
    }
}