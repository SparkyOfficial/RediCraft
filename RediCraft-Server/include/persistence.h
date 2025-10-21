/*
 * persistence.h
 * author: Андрій Будильников
 */

#ifndef REDICRAFT_PERSISTENCE_H
#define REDICRAFT_PERSISTENCE_H

#include "storage.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <future>
#include <memory>

class PersistenceManager {
public:
    PersistenceManager(Storage& storage);
    ~PersistenceManager();
    
    // Load data from file (blocking)
    bool loadFromFile(const std::string& filename);
    
    // Save data to file (blocking)
    bool saveToFile(const std::string& filename);
    
    // Create a snapshot of current data for persistence
    std::tuple<
        std::unordered_map<std::string, Storage::DataItem>,
        std::unordered_map<std::string, Storage::HashItem>,
        std::unordered_map<std::string, Storage::ListItem>,
        std::unordered_map<std::string, Storage::SetItem>
    > createSnapshot();
    
    // Save data to file asynchronously (non-blocking)
    std::future<bool> saveToFileAsync(const std::string& filename);
    
    // Start automatic persistence
    void startAutoPersistence(const std::string& filename, int interval_seconds);
    
    // Stop automatic persistence
    void stopAutoPersistence();
    
private:
    Storage& storage_;
    std::atomic<bool> auto_persistence_running_;
    std::thread auto_persistence_thread_;
    
    // Thread pool for async operations
    std::vector<std::thread> worker_threads_;
    std::atomic<bool> workers_running_;
    
    // Helper methods for serialization
    std::string serializeStringData(const std::unordered_map<std::string, Storage::DataItem>& data);
    std::string serializeHashData(const std::unordered_map<std::string, Storage::HashItem>& data);
    std::string serializeListData(const std::unordered_map<std::string, Storage::ListItem>& data);
    
    // Helper methods for deserialization
    void deserializeStringData(const std::string& data);
    void deserializeHashData(const std::string& data);
    void deserializeListData(const std::string& data);
    
    // Automatic persistence loop
    void autoPersistenceLoop(const std::string& filename, int interval_seconds);
    
    // Initialize worker threads
    void initializeWorkers();
    
    // Shutdown worker threads
    void shutdownWorkers();
};

#endif // REDICRAFT_PERSISTENCE_H