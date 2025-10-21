/*
 * storage.cpp
 * author: Андрій Будильников
 */

#include "storage.h"
#include <shared_mutex>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>
#include <chrono>

Storage::Storage() = default;

bool Storage::is_expired(const std::chrono::steady_clock::time_point& expiry) const {
    return expiry.time_since_epoch().count() > 0 && 
           std::chrono::steady_clock::now() > expiry;
}

void Storage::remove_expired(const std::string& key) {
    // Check string data
    auto string_it = string_data_.find(key);
    if (string_it != string_data_.end() && 
        string_it->second.has_expiry && 
        is_expired(string_it->second.expiry)) {
        string_data_.erase(string_it);
    }
    
    // Check hash data
    auto hash_it = hash_data_.find(key);
    if (hash_it != hash_data_.end() && 
        hash_it->second.has_expiry && 
        is_expired(hash_it->second.expiry)) {
        hash_data_.erase(hash_it);
    }
    
    // Check list data
    auto list_it = list_data_.find(key);
    if (list_it != list_data_.end() && 
        list_it->second.has_expiry && 
        is_expired(list_it->second.expiry)) {
        list_data_.erase(list_it);
    }
    
    // Check set data
    auto set_it = set_data_.find(key);
    if (set_it != set_data_.end() && 
        set_it->second.has_expiry && 
        is_expired(set_it->second.expiry)) {
        set_data_.erase(set_it);
    }
}

bool Storage::set(const std::string& key, const std::string& value) {
    std::unique_lock lock(mutex_);
    remove_expired(key);
    
    DataItem item(value);
    string_data_[key] = item;
    return true;
}

bool Storage::get(const std::string& key, std::string& value) {
    std::shared_lock lock(mutex_);
    remove_expired(key);
    
    auto it = string_data_.find(key);
    if (it != string_data_.end()) {
        value = it->second.value;
        return true;
    }
    return false;
}

bool Storage::ping() {
    // Just return true to indicate we're alive
    return true;
}

long long Storage::incr(const std::string& key) {
    std::unique_lock lock(mutex_);
    remove_expired(key);
    
    auto it = string_data_.find(key);
    if (it != string_data_.end()) {
        try {
            long long value = std::stoll(it->second.value);
            value++;
            it->second.value = std::to_string(value);
            return value;
        } catch (const std::exception&) {
            // If the value is not a valid number, treat it as 0
            DataItem item("1");
            string_data_[key] = item;
            return 1;
        }
    } else {
        // Key doesn't exist, create it with value 1
        DataItem item("1");
        string_data_[key] = item;
        return 1;
    }
}

long long Storage::decr(const std::string& key) {
    std::unique_lock lock(mutex_);
    remove_expired(key);
    
    auto it = string_data_.find(key);
    if (it != string_data_.end()) {
        try {
            long long value = std::stoll(it->second.value);
            value--;
            it->second.value = std::to_string(value);
            return value;
        } catch (const std::exception&) {
            // If the value is not a valid number, treat it as 0
            DataItem item("-1");
            string_data_[key] = item;
            return -1;
        }
    } else {
        // Key doesn't exist, create it with value -1
        DataItem item("-1");
        string_data_[key] = item;
        return -1;
    }
}

long long Storage::incrby(const std::string& key, long long increment) {
    std::unique_lock lock(mutex_);
    remove_expired(key);
    
    auto it = string_data_.find(key);
    if (it != string_data_.end()) {
        try {
            long long value = std::stoll(it->second.value);
            value += increment;
            it->second.value = std::to_string(value);
            return value;
        } catch (const std::exception&) {
            // If the value is not a valid number, treat it as 0
            long long newValue = increment;
            DataItem item(std::to_string(newValue));
            string_data_[key] = item;
            return newValue;
        }
    } else {
        // Key doesn't exist, create it with the increment value
        DataItem item(std::to_string(increment));
        string_data_[key] = item;
        return increment;
    }
}

bool Storage::hset(const std::string& key, const std::string& field, const std::string& value) {
    std::unique_lock lock(mutex_);
    remove_expired(key);
    
    auto it = hash_data_.find(key);
    if (it != hash_data_.end()) {
        // Hash exists, update field
        it->second.fields[field] = value;
    } else {
        // Create new hash
        HashItem item;
        item.fields[field] = value;
        hash_data_[key] = item;
    }
    return true;
}

bool Storage::hget(const std::string& key, const std::string& field, std::string& value) {
    std::shared_lock lock(mutex_);
    remove_expired(key);
    
    auto it = hash_data_.find(key);
    if (it != hash_data_.end()) {
        auto field_it = it->second.fields.find(field);
        if (field_it != it->second.fields.end()) {
            value = field_it->second;
            return true;
        }
    }
    return false;
}

std::unordered_map<std::string, std::string> Storage::hgetall(const std::string& key) {
    std::shared_lock lock(mutex_);
    remove_expired(key);
    
    auto it = hash_data_.find(key);
    if (it != hash_data_.end()) {
        return it->second.fields;
    }
    return std::unordered_map<std::string, std::string>();
}

long long Storage::lpush(const std::string& key, const std::vector<std::string>& values) {
    std::unique_lock lock(mutex_);
    remove_expired(key);
    
    auto it = list_data_.find(key);
    if (it != list_data_.end()) {
        // List exists, prepend values
        for (auto rit = values.rbegin(); rit != values.rend(); ++rit) {
            it->second.values.insert(it->second.values.begin(), *rit);
        }
    } else {
        // Create new list
        ListItem item;
        item.values = values;
        list_data_[key] = item;
    }
    
    // Return the new length of the list
    auto list_it = list_data_.find(key);
    return list_it != list_data_.end() ? static_cast<long long>(list_it->second.values.size()) : 0;
}

bool Storage::rpop(const std::string& key, std::string& value) {
    std::unique_lock lock(mutex_);
    remove_expired(key);
    
    auto it = list_data_.find(key);
    if (it != list_data_.end() && !it->second.values.empty()) {
        value = it->second.values.back();
        it->second.values.pop_back();
        return true;
    }
    return false;
}

std::vector<std::string> Storage::lrange(const std::string& key, long long start, long long end) {
    std::shared_lock lock(mutex_);
    remove_expired(key);
    
    auto it = list_data_.find(key);
    if (it != list_data_.end() && !it->second.values.empty()) {
        const auto& values = it->second.values;
        long long size = static_cast<long long>(values.size());
        
        // Handle negative indices
        if (start < 0) start = size + start;
        if (end < 0) end = size + end;
        
        // Clamp indices to valid range
        start = std::max(0LL, std::min(start, size - 1));
        end = std::max(0LL, std::min(end, size - 1));
        
        // Ensure start <= end
        if (start > end) {
            return std::vector<std::string>();
        }
        
        // Extract the range
        std::vector<std::string> result;
        for (long long i = start; i <= end && i < size; ++i) {
            result.push_back(values[i]);
        }
        return result;
    }
    return std::vector<std::string>();
}

// Set operations
long long Storage::sadd(const std::string& key, const std::vector<std::string>& members) {
    std::unique_lock lock(mutex_);
    remove_expired(key);
    
    long long added = 0;
    auto it = set_data_.find(key);
    if (it != set_data_.end()) {
        // Set exists, add members
        for (const auto& member : members) {
            if (it->second.members.insert(member).second) {
                added++;
            }
        }
    } else {
        // Create new set
        SetItem item;
        for (const auto& member : members) {
            item.members.insert(member);
        }
        set_data_[key] = item;
        added = members.size();
    }
    
    return added;
}

long long Storage::srem(const std::string& key, const std::vector<std::string>& members) {
    std::unique_lock lock(mutex_);
    remove_expired(key);
    
    long long removed = 0;
    auto it = set_data_.find(key);
    if (it != set_data_.end()) {
        // Set exists, remove members
        for (const auto& member : members) {
            if (it->second.members.erase(member)) {
                removed++;
            }
        }
    }
    
    return removed;
}

bool Storage::sismember(const std::string& key, const std::string& member) {
    std::shared_lock lock(mutex_);
    remove_expired(key);
    
    auto it = set_data_.find(key);
    if (it != set_data_.end()) {
        return it->second.members.find(member) != it->second.members.end();
    }
    return false;
}

std::unordered_set<std::string> Storage::smembers(const std::string& key) {
    std::shared_lock lock(mutex_);
    remove_expired(key);
    
    auto it = set_data_.find(key);
    if (it != set_data_.end()) {
        return it->second.members;
    }
    return std::unordered_set<std::string>();
}

long long Storage::scard(const std::string& key) {
    std::shared_lock lock(mutex_);
    remove_expired(key);
    
    auto it = set_data_.find(key);
    if (it != set_data_.end()) {
        return static_cast<long long>(it->second.members.size());
    }
    return 0;
}

bool Storage::expire(const std::string& key, long long seconds) {
    std::unique_lock lock(mutex_);
    remove_expired(key);
    
    // Check all data types
    auto string_it = string_data_.find(key);
    if (string_it != string_data_.end()) {
        string_it->second.has_expiry = true;
        string_it->second.expiry = std::chrono::steady_clock::now() + 
                                  std::chrono::seconds(seconds);
        return true;
    }
    
    auto hash_it = hash_data_.find(key);
    if (hash_it != hash_data_.end()) {
        hash_it->second.has_expiry = true;
        hash_it->second.expiry = std::chrono::steady_clock::now() + 
                                std::chrono::seconds(seconds);
        return true;
    }
    
    auto list_it = list_data_.find(key);
    if (list_it != list_data_.end()) {
        list_it->second.has_expiry = true;
        list_it->second.expiry = std::chrono::steady_clock::now() + 
                                std::chrono::seconds(seconds);
        return true;
    }
    
    auto set_it = set_data_.find(key);
    if (set_it != set_data_.end()) {
        set_it->second.has_expiry = true;
        set_it->second.expiry = std::chrono::steady_clock::now() + 
                               std::chrono::seconds(seconds);
        return true;
    }
    
    return false;
}

long long Storage::ttl(const std::string& key) {
    std::shared_lock lock(mutex_);
    remove_expired(key);
    
    // Check all data types
    auto string_it = string_data_.find(key);
    if (string_it != string_data_.end() && string_it->second.has_expiry) {
        auto now = std::chrono::steady_clock::now();
        if (string_it->second.expiry > now) {
            auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
                string_it->second.expiry - now);
            return remaining.count();
        }
        return -1; // Key exists but expired
    }
    
    auto hash_it = hash_data_.find(key);
    if (hash_it != hash_data_.end() && hash_it->second.has_expiry) {
        auto now = std::chrono::steady_clock::now();
        if (hash_it->second.expiry > now) {
            auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
                hash_it->second.expiry - now);
            return remaining.count();
        }
        return -1; // Key exists but expired
    }
    
    auto list_it = list_data_.find(key);
    if (list_it != list_data_.end() && list_it->second.has_expiry) {
        auto now = std::chrono::steady_clock::now();
        if (list_it->second.expiry > now) {
            auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
                list_it->second.expiry - now);
            return remaining.count();
        }
        return -1; // Key exists but expired
    }
    
    auto set_it = set_data_.find(key);
    if (set_it != set_data_.end() && set_it->second.has_expiry) {
        auto now = std::chrono::steady_clock::now();
        if (set_it->second.expiry > now) {
            auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
                set_it->second.expiry - now);
            return remaining.count();
        }
        return -1; // Key exists but expired
    }
    
    // Key doesn't exist or has no expiry
    auto string_check = string_data_.find(key);
    if (string_check != string_data_.end()) return -1; // Key exists but no expiry
    
    auto hash_check = hash_data_.find(key);
    if (hash_check != hash_data_.end()) return -1; // Key exists but no expiry
    
    auto list_check = list_data_.find(key);
    if (list_check != list_data_.end()) return -1; // Key exists but no expiry
    
    auto set_check = set_data_.find(key);
    if (set_check != set_data_.end()) return -1; // Key exists but no expiry
    
    return -2; // Key doesn't exist
}

// Persistence methods
const std::unordered_map<std::string, Storage::DataItem>& Storage::getStringData() const {
    return string_data_;
}

const std::unordered_map<std::string, Storage::HashItem>& Storage::getHashData() const {
    return hash_data_;
}

const std::unordered_map<std::string, Storage::ListItem>& Storage::getListData() const {
    return list_data_;
}

const std::unordered_map<std::string, Storage::SetItem>& Storage::getSetData() const {
    return set_data_;
}