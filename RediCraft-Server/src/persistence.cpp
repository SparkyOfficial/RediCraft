/*
 * persistence.cpp
 * author: Андрій Будильников
 */

#include "../include/persistence.h"
#include "../include/storage.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <future>

PersistenceManager::PersistenceManager(Storage& storage)
    : storage_(storage)
    , auto_persistence_running_(false)
    , workers_running_(false) {
    initializeWorkers();
}

PersistenceManager::~PersistenceManager() {
    stopAutoPersistence();
    shutdownWorkers();
}

void PersistenceManager::initializeWorkers() {
    workers_running_ = true;
    // Create a small thread pool for async operations
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 2;
    
    for (unsigned int i = 0; i < num_threads; ++i) {
        worker_threads_.emplace_back([this]() {
            while (workers_running_) {
                // Worker threads can be used for async operations
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }
}

void PersistenceManager::shutdownWorkers() {
    workers_running_ = false;
    for (auto& thread : worker_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    worker_threads_.clear();
}

bool PersistenceManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open file for reading: " << filename << std::endl;
        return false;
    }
    
    // For now, we'll implement a simple text-based format
    // In a production system, you might want to use a more efficient binary format
    std::string line;
    std::string section;
    
    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Check for section headers
        if (line[0] == '[' && line[line.length()-1] == ']') {
            section = line.substr(1, line.length() - 2);
            continue;
        }
        
        // Parse key-value pairs
        size_t equals_pos = line.find('=');
        if (equals_pos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, equals_pos);
        std::string value = line.substr(equals_pos + 1);
        
        // Process based on section
        if (section == "STRINGS") {
            storage_.set(key, value);
        }
        // For hashes and lists, we would need more complex parsing
        // This is a simplified implementation
    }
    
    return true;
}

// Create a snapshot of the current data without holding locks during file I/O
std::tuple<
    std::unordered_map<std::string, Storage::DataItem>,
    std::unordered_map<std::string, Storage::HashItem>,
    std::unordered_map<std::string, Storage::ListItem>,
    std::unordered_map<std::string, Storage::SetItem>
> PersistenceManager::createSnapshot() {
    // Create copies of all data structures
    auto stringData = storage_.getStringData();
    auto hashData = storage_.getHashData();
    auto listData = storage_.getListData();
    auto setData = storage_.getSetData();
    
    return std::make_tuple(std::move(stringData), std::move(hashData), std::move(listData), std::move(setData));
}

bool PersistenceManager::saveToFile(const std::string& filename) {
    // Create a snapshot of the current data
    // This avoids holding locks during the file I/O operation
    auto [stringData, hashData, listData, setData] = createSnapshot();
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open file for writing: " << filename << std::endl;
        return false;
    }
    
    // Write string data
    file << "[STRINGS]\n";
    for (const auto& pair : stringData) {
        // Only save non-expired items
        if (!pair.second.has_expiry || pair.second.expiry > std::chrono::steady_clock::now()) {
            file << pair.first << "=" << pair.second.value << "\n";
        }
    }
    
    file << "[HASHES]\n";
    for (const auto& pair : hashData) {
        // Only save non-expired items
        if (!pair.second.has_expiry || pair.second.expiry > std::chrono::steady_clock::now()) {
            for (const auto& field : pair.second.fields) {
                file << pair.first << "." << field.first << "=" << field.second << "\n";
            }
        }
    }
    
    file << "[LISTS]\n";
    for (const auto& pair : listData) {
        // Only save non-expired items
        if (!pair.second.has_expiry || pair.second.expiry > std::chrono::steady_clock::now()) {
            for (size_t i = 0; i < pair.second.values.size(); ++i) {
                file << pair.first << "[" << i << "]=" << pair.second.values[i] << "\n";
            }
        }
    }
    
    file << "[SETS]\n";
    for (const auto& pair : setData) {
        // Only save non-expired items
        if (!pair.second.has_expiry || pair.second.expiry > std::chrono::steady_clock::now()) {
            for (const auto& member : pair.second.members) {
                file << pair.first << "." << member.first << "=1\n";
            }
        }
    }
    
    return true;
}

std::future<bool> PersistenceManager::saveToFileAsync(const std::string& filename) {
    // Create a promise and future for the async operation
    auto promise = std::make_shared<std::promise<bool>>();
    auto future = promise->get_future();
    
    // Launch the operation in a separate thread
    std::thread([this, filename, promise]() {
        try {
            bool result = saveToFile(filename);
            promise->set_value(result);
        } catch (...) {
            promise->set_exception(std::current_exception());
        }
    }).detach();
    
    return future;
}

void PersistenceManager::startAutoPersistence(const std::string& filename, int interval_seconds) {
    if (auto_persistence_running_) {
        return;
    }
    
    auto_persistence_running_ = true;
    auto_persistence_thread_ = std::thread(&PersistenceManager::autoPersistenceLoop, this, filename, interval_seconds);
}

void PersistenceManager::stopAutoPersistence() {
    if (auto_persistence_running_) {
        auto_persistence_running_ = false;
        if (auto_persistence_thread_.joinable()) {
            auto_persistence_thread_.join();
        }
    }
}

void PersistenceManager::autoPersistenceLoop(const std::string& filename, int interval_seconds) {
    while (auto_persistence_running_) {
        // Save data asynchronously to avoid blocking
        auto future = saveToFileAsync(filename);
        
        // Wait for completion or timeout
        try {
            future.wait_for(std::chrono::seconds(30)); // 30 second timeout
        } catch (const std::exception& e) {
            std::cerr << "Error during auto persistence: " << e.what() << std::endl;
        }
        
        // Sleep for the specified interval
        for (int i = 0; i < interval_seconds && auto_persistence_running_; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

std::string PersistenceManager::serializeStringData(const std::unordered_map<std::string, Storage::DataItem>& data) {
    // Placeholder implementation
    return "";
}

std::string PersistenceManager::serializeHashData(const std::unordered_map<std::string, Storage::HashItem>& data) {
    // Placeholder implementation
    return "";
}

std::string PersistenceManager::serializeListData(const std::unordered_map<std::string, Storage::ListItem>& data) {
    // Placeholder implementation
    return "";
}

void PersistenceManager::deserializeStringData(const std::string& data) {
    // Placeholder implementation
}

void PersistenceManager::deserializeHashData(const std::string& data) {
    // Placeholder implementation
}

void PersistenceManager::deserializeListData(const std::string& data) {
    // Placeholder implementation
}