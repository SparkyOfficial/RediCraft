/*
 * storage.h
 * author: Андрій Будильников
 */

#ifndef REDICRAFT_STORAGE_H
#define REDICRAFT_STORAGE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>
#include <vector>
#include <chrono>

class Storage {
public:
    // Forward declare the internal structures
    struct DataItem;
    struct HashItem;
    struct ListItem;
    struct SetItem;
    
    Storage();
    
    // Basic key-value operations
    bool set(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& value);
    bool ping();
    
    // Numeric operations
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
    std::unordered_set<std::string> smembers(const std::string& key);
    long long scard(const std::string& key);
    
    // Expiration operations
    bool expire(const std::string& key, long long seconds);
    long long ttl(const std::string& key);

    // Persistence operations
    const std::unordered_map<std::string, DataItem>& getStringData() const;
    const std::unordered_map<std::string, HashItem>& getHashData() const;
    const std::unordered_map<std::string, ListItem>& getListData() const;
    const std::unordered_map<std::string, SetItem>& getSetData() const;

    // Struct definitions
    struct DataItem {
        std::string value;
        std::chrono::steady_clock::time_point expiry;
        bool has_expiry;
        
        DataItem() : has_expiry(false) {
            expiry = std::chrono::steady_clock::time_point();
        }
        DataItem(const std::string& v) : value(v), has_expiry(false) {
            expiry = std::chrono::steady_clock::time_point();
        }
    };
    
    struct HashItem {
        std::unordered_map<std::string, std::string> fields;
        std::chrono::steady_clock::time_point expiry;
        bool has_expiry;
        
        HashItem() : has_expiry(false) {
            expiry = std::chrono::steady_clock::time_point();
        }
    };
    
    struct ListItem {
        std::vector<std::string> values;
        std::chrono::steady_clock::time_point expiry;
        bool has_expiry;
        
        ListItem() : has_expiry(false) {
            expiry = std::chrono::steady_clock::time_point();
        }
    };
    
    struct SetItem {
        std::unordered_set<std::string> members;
        std::chrono::steady_clock::time_point expiry;
        bool has_expiry;
        
        SetItem() : has_expiry(false) {
            expiry = std::chrono::steady_clock::time_point();
        }
    };

private:
    // Different storage types for different data structures
    std::unordered_map<std::string, DataItem> string_data_;
    std::unordered_map<std::string, HashItem> hash_data_;
    std::unordered_map<std::string, ListItem> list_data_;
    std::unordered_map<std::string, SetItem> set_data_;
    
    mutable std::shared_mutex mutex_;
    
    // Helper methods
    bool is_expired(const std::chrono::steady_clock::time_point& expiry) const;
    void remove_expired(const std::string& key);
};

#endif // REDICRAFT_STORAGE_H