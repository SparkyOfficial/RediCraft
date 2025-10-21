/*
 * RedicraftException.java
 * author: Андрій Будильников
 */

package com.redicraft;

import java.io.IOException;

/**
 * Custom exception class for RediCraft client operations
 */
public class RedicraftException extends IOException {
    
    /**
     * Constructs a new exception with the specified detail message.
     *
     * @param message the detail message
     */
    public RedicraftException(String message) {
        super(message);
    }
    
    /**
     * Constructs a new exception with the specified detail message and cause.
     *
     * @param message the detail message
     * @param cause the cause of the exception
     */
    public RedicraftException(String message, Throwable cause) {
        super(message, cause);
    }
    
    /**
     * Constructs a new exception with the specified cause.
     *
     * @param cause the cause of the exception
     */
    public RedicraftException(Throwable cause) {
        super(cause);
    }
    
    /**
     * Unchecked version of RedicraftException for use in functional interfaces
     */
    public static class Unchecked extends RuntimeException {
        
        /**
         * Constructs a new unchecked exception with the specified detail message and cause.
         *
         * @param message the detail message
         * @param cause the cause of the exception
         */
        public Unchecked(String message, Throwable cause) {
            super(message, cause);
        }
        
        /**
         * Constructs a new unchecked exception with the specified cause.
         *
         * @param cause the cause of the exception
         */
        public Unchecked(Throwable cause) {
            super(cause);
        }
        
        /**
         * Gets the underlying RedicraftException
         * @return the underlying RedicraftException
         */
        public RedicraftException getRedicraftException() {
            Throwable cause = getCause();
            if (cause instanceof RedicraftException) {
                return (RedicraftException) cause;
            }
            return new RedicraftException(cause);
        }
    }
}