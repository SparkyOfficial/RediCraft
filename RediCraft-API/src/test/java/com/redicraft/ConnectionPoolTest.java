/*
 * ConnectionPoolTest.java
 * author: Андрій Будильников
 */

package com.redicraft;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

public class ConnectionPoolTest {
    
    @Test
    public void testPoolCreation() {
        ConnectionPool pool = new ConnectionPool("localhost", 7379, 5);
        assertNotNull(pool);
        assertEquals(0, pool.getActiveConnections());
        assertEquals(0, pool.getAvailableConnections());
    }
    
    @Test
    public void testPoolMaxConnections() {
        ConnectionPool pool = new ConnectionPool("localhost", 7379, 2);
        assertNotNull(pool);
        
        // The pool should allow up to maxConnections
        // Note: This test would require a running server to be fully effective
        assertEquals(0, pool.getActiveConnections());
        assertEquals(0, pool.getAvailableConnections());
    }
    
    // Note: Integration tests that actually use the pool
    // would require the server to be running and are not included here
}