/*
 * storage.h
 * author: Андрій Будильников
 */

#ifndef REDICRAFT_STORAGE_H
#define REDICRAFT_STORAGE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <chrono>

class Storage {
public:
    struct DataItem {
        std::string value;
        bool has_expiry = false;
        std::chrono::steady_clock::time_point expiry;
        
        DataItem() = default;
        explicit DataItem(const std::string& val) : value(val) {}
    };
    
    struct HashItem {
        std::unordered_map<std::string, std::string> fields;
        bool has_expiry = false;
        std::chrono::steady_clock::time_point expiry;
    };
    
    struct ListItem {
        std::vector<std::string> values;
        bool has_expiry = false;
        std::chrono::steady_clock::time_point expiry;
    };
    
    struct SetItem {
        std::unordered_map<std::string, bool> members; // Using map for O(1) lookup
        bool has_expiry = false;
        std::chrono::steady_clock::time_point expiry;
    };

    Storage();
    
    // String operations
    bool set(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& value);
    long long incr(const std::string& key);
    long long decr(const std::string& key);
    long long incrby(const std::string& key, long long increment);
    
    // Hash operations
    bool hset(const std::string& key, const std::string& field, const std::string& value);
    bool hget(const std::string& key, const std::string& field, std::string& value);
    std::unordered_map<std::string, std::string> hgetall(const std::string& key);
    
    // List operations
    long long lpush(const std::string& key, const std::vector<std::string>& values);
    bool rpop(const std::string& key, std::string& value);
    std::vector<std::string> lrange(const std::string& key, long long start, long long end);
    
    // Set operations
    long long sadd(const std::string& key, const std::vector<std::string>& members);
    long long srem(const std::string& key, const std::vector<std::string>& members);
    bool sismember(const std::string& key, const std::string& member);
    std::unordered_map<std::string, bool> smembers(const std::string& key);
    long long scard(const std::string& key);
    
    // Expiration
    bool expire(const std::string& key, long long seconds);
    long long ttl(const std::string& key);
    
    // Utility
    bool ping();
    
    // Data access for persistence and testing
    const std::unordered_map<std::string, DataItem>& getStringData() const { return string_data_; }
    const std::unordered_map<std::string, HashItem>& getHashData() const { return hash_data_; }
    const std::unordered_map<std::string, ListItem>& getListData() const { return list_data_; }
    const std::unordered_map<std::string, SetItem>& getSetData() const { return set_data_; }
    
private:
    // Data storage with separate mutexes for each type to reduce contention
    std::unordered_map<std::string, DataItem> string_data_;
    std::unordered_map<std::string, HashItem> hash_data_;
    std::unordered_map<std::string, ListItem> list_data_;
    std::unordered_map<std::string, SetItem> set_data_;
    
    // Separate mutexes for each data type to improve concurrency
    mutable std::shared_mutex string_mutex_;
    mutable std::shared_mutex hash_mutex_;
    mutable std::shared_mutex list_mutex_;
    mutable std::shared_mutex set_mutex_;
    
    // Helper methods
    bool is_expired(const std::chrono::steady_clock::time_point& expiry) const;
    void remove_expired(const std::string& key);
};

#endif // REDICRAFT_STORAGE_H