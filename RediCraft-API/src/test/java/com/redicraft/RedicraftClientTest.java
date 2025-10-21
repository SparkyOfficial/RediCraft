/*
 * RedicraftClientTest.java
 * author: Андрій Будильников
 */

package com.redicraft;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

public class RedicraftClientTest {
    
    @Test
    public void testClientCreation() {
        RedicraftClient client = new RedicraftClient();
        assertNotNull(client);
    }
    
    @Test
    public void testConnectionPoolCreation() {
        ConnectionPool pool = new ConnectionPool("localhost", 7379, 5);
        assertNotNull(pool);
        assertEquals(0, pool.getActiveConnections());
        assertEquals(0, pool.getAvailableConnections());
    }
    
    // Note: Integration tests that actually connect to a server
    // would require the server to be running and are not included here
    
    /*
    @Test
    public void testIncrCommand() throws IOException {
        RedicraftClient client = new RedicraftClient();
        client.connect("localhost", 7379);
        
        // Test INCR on a non-existent key (should return 1)
        long result = client.incr("test_counter");
        assertEquals(1L, result);
        
        // Test INCR on an existing key (should return 2)
        result = client.incr("test_counter");
        assertEquals(2L, result);
        
        client.close();
    }
    
    @Test
    public void testDecrCommand() throws IOException {
        RedicraftClient client = new RedicraftClient();
        client.connect("localhost", 7379);
        
        // First set a value
        client.set("test_counter", "5");
        
        // Test DECR (should return 4)
        long result = client.decr("test_counter");
        assertEquals(4L, result);
        
        client.close();
    }
    
    @Test
    public void testIncrByCommand() throws IOException {
        RedicraftClient client = new RedicraftClient();
        client.connect("localhost", 7379);
        
        // Test INCRBY on a non-existent key
        long result = client.incrBy("test_counter", 10);
        assertEquals(10L, result);
        
        // Test INCRBY on an existing key
        result = client.incrBy("test_counter", 5);
        assertEquals(15L, result);
        
        client.close();
    }
    */
}