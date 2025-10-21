/*
 * benchmark.cpp
 * author: Андрій Будильников
 */

#include "../include/storage.h"
#include <iostream>
#include <chrono>
#include <string>
#include <random>

int main() {
    Storage storage;
    
    // Test parameters
    const int num_operations = 100000;
    const int key_range = 1000;
    
    std::cout << "Starting RediCraft benchmark...\n";
    std::cout << "Operations: " << num_operations << "\n";
    std::cout << "Key range: " << key_range << "\n\n";
    
    // Generate random keys and values
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> key_dist(0, key_range - 1);
    std::uniform_int_distribution<> value_dist(0, 1000000);
    
    // Benchmark SET operations
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_operations; ++i) {
        std::string key = "key:" + std::to_string(key_dist(gen));
        std::string value = std::to_string(value_dist(gen));
        storage.set(key, value);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "SET operations:\n";
    std::cout << "  Time: " << duration.count() << " ms\n";
    std::cout << "  Operations per second: " << (num_operations * 1000.0 / duration.count()) << "\n\n";
    
    // Benchmark GET operations
    start = std::chrono::high_resolution_clock::now();
    
    int found = 0;
    for (int i = 0; i < num_operations; ++i) {
        std::string key = "key:" + std::to_string(key_dist(gen));
        std::string value;
        if (storage.get(key, value)) {
            found++;
        }
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "GET operations:\n";
    std::cout << "  Time: " << duration.count() << " ms\n";
    std::cout << "  Operations per second: " << (num_operations * 1000.0 / duration.count()) << "\n";
    std::cout << "  Hit rate: " << (found * 100.0 / num_operations) << "%\n\n";
    
    // Benchmark INCR operations
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_operations; ++i) {
        std::string key = "counter:" + std::to_string(key_dist(gen));
        storage.incr(key);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "INCR operations:\n";
    std::cout << "  Time: " << duration.count() << " ms\n";
    std::cout << "  Operations per second: " << (num_operations * 1000.0 / duration.count()) << "\n\n";
    
    std::cout << "Benchmark completed!\n";
    
    return 0;
}