/*
 * storage.cpp
 * author: Андрій Будильников
 */

#include "../include/storage.h"
#include <shared_mutex>
#include <mutex>
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
    {
        std::shared_lock<std::shared_mutex> lock(string_mutex_);
        auto string_it = string_data_.find(key);
        if (string_it != string_data_.end() && 
            string_it->second.has_expiry && 
            is_expired(string_it->second.expiry)) {
            string_mutex_.unlock();
            std::unique_lock<std::shared_mutex> unique_lock(string_mutex_);
            string_data_.erase(string_it);
        }
    }
    
    // Check hash data
    {
        std::shared_lock<std::shared_mutex> lock(hash_mutex_);
        auto hash_it = hash_data_.find(key);
        if (hash_it != hash_data_.end() && 
            hash_it->second.has_expiry && 
            is_expired(hash_it->second.expiry)) {
            hash_mutex_.unlock();
            std::unique_lock<std::shared_mutex> unique_lock(hash_mutex_);
            hash_data_.erase(hash_it);
        }
    }
    
    // Check list data
    {
        std::shared_lock<std::shared_mutex> lock(list_mutex_);
        auto list_it = list_data_.find(key);
        if (list_it != list_data_.end() && 
            list_it->second.has_expiry && 
            is_expired(list_it->second.expiry)) {
            list_mutex_.unlock();
            std::unique_lock<std::shared_mutex> unique_lock(list_mutex_);
            list_data_.erase(list_it);
        }
    }
    
    // Check set data
    {
        std::shared_lock<std::shared_mutex> lock(set_mutex_);
        auto set_it = set_data_.find(key);
        if (set_it != set_data_.end() && 
            set_it->second.has_expiry && 
            is_expired(set_it->second.expiry)) {
            set_mutex_.unlock();
            std::unique_lock<std::shared_mutex> unique_lock(set_mutex_);
            set_data_.erase(set_it);
        }
    }
}

bool Storage::set(const std::string& key, const std::string& value) {
    std::unique_lock<std::shared_mutex> lock(string_mutex_);
    remove_expired(key);
    
    DataItem item(value);
    string_data_[key] = item;
    return true;
}

bool Storage::get(const std::string& key, std::string& value) {
    std::shared_lock<std::shared_mutex> lock(string_mutex_);
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
    std::unique_lock<std::shared_mutex> lock(string_mutex_);
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
    std::unique_lock<std::shared_mutex> lock(string_mutex_);
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
    std::unique_lock<std::shared_mutex> lock(string_mutex_);
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
    std::unique_lock<std::shared_mutex> lock(hash_mutex_);
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
    std::shared_lock<std::shared_mutex> lock(hash_mutex_);
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
    std::shared_lock<std::shared_mutex> lock(hash_mutex_);
    remove_expired(key);
    
    auto it = hash_data_.find(key);
    if (it != hash_data_.end()) {
        return it->second.fields;
    }
    return std::unordered_map<std::string, std::string>();
}

long long Storage::lpush(const std::string& key, const std::vector<std::string>& values) {
    std::unique_lock<std::shared_mutex> lock(list_mutex_);
    remove_expired(key);
    
    auto it = list_data_.find(key);
    if (it != list_data_.end()) {
        // List exists, prepend values
        it->second.values.insert(it->second.values.begin(), values.begin(), values.end());
        return static_cast<long long>(it->second.values.size());
    } else {
        // Create new list
        ListItem item;
        item.values.insert(item.values.begin(), values.begin(), values.end());
        list_data_[key] = item;
        return static_cast<long long>(values.size());
    }
}

bool Storage::rpop(const std::string& key, std::string& value) {
    std::unique_lock<std::shared_mutex> lock(list_mutex_);
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
    std::shared_lock<std::shared_mutex> lock(list_mutex_);
    remove_expired(key);
    
    auto it = list_data_.find(key);
    if (it != list_data_.end()) {
        const auto& values = it->second.values;
        if (values.empty()) {
            return {};
        }
        
        // Handle negative indices
        long long size = static_cast<long long>(values.size());
        if (start < 0) start = size + start;
        if (end < 0) end = size + end;
        
        // Clamp indices to valid range
        start = std::max(0LL, std::min(start, size - 1));
        end = std::max(0LL, std::min(end, size - 1));
        
        // Ensure start <= end
        if (start > end) {
            return {};
        }
        
        // Return the range
        return std::vector<std::string>(values.begin() + start, values.begin() + end + 1);
    }
    return {};
}

long long Storage::sadd(const std::string& key, const std::vector<std::string>& members) {
    std::unique_lock<std::shared_mutex> lock(set_mutex_);
    remove_expired(key);
    
    long long added = 0;
    auto it = set_data_.find(key);
    
    if (it != set_data_.end()) {
        // Set exists, add members
        for (const auto& member : members) {
            if (it->second.members.find(member) == it->second.members.end()) {
                it->second.members[member] = true;
                added++;
            }
        }
    } else {
        // Create new set
        SetItem item;
        for (const auto& member : members) {
            item.members[member] = true;
            added++;
        }
        set_data_[key] = item;
    }
    
    return added;
}

long long Storage::srem(const std::string& key, const std::vector<std::string>& members) {
    std::unique_lock<std::shared_mutex> lock(set_mutex_);
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
        
        // If set is now empty, remove it entirely
        if (it->second.members.empty()) {
            set_data_.erase(it);
        }
    }
    
    return removed;
}

bool Storage::sismember(const std::string& key, const std::string& member) {
    std::shared_lock<std::shared_mutex> lock(set_mutex_);
    remove_expired(key);
    
    auto it = set_data_.find(key);
    if (it != set_data_.end()) {
        return it->second.members.find(member) != it->second.members.end();
    }
    return false;
}

std::unordered_map<std::string, bool> Storage::smembers(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(set_mutex_);
    remove_expired(key);
    
    auto it = set_data_.find(key);
    if (it != set_data_.end()) {
        return it->second.members;
    }
    return std::unordered_map<std::string, bool>();
}

long long Storage::scard(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(set_mutex_);
    remove_expired(key);
    
    auto it = set_data_.find(key);
    if (it != set_data_.end()) {
        return static_cast<long long>(it->second.members.size());
    }
    return 0;
}

bool Storage::expire(const std::string& key, long long seconds) {
    auto expiry_time = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    
    // Check each data type
    {
        std::unique_lock<std::shared_mutex> lock(string_mutex_);
        auto it = string_data_.find(key);
        if (it != string_data_.end()) {
            it->second.has_expiry = true;
            it->second.expiry = expiry_time;
            return true;
        }
    }
    
    {
        std::unique_lock<std::shared_mutex> lock(hash_mutex_);
        auto it = hash_data_.find(key);
        if (it != hash_data_.end()) {
            it->second.has_expiry = true;
            it->second.expiry = expiry_time;
            return true;
        }
    }
    
    {
        std::unique_lock<std::shared_mutex> lock(list_mutex_);
        auto it = list_data_.find(key);
        if (it != list_data_.end()) {
            it->second.has_expiry = true;
            it->second.expiry = expiry_time;
            return true;
        }
    }
    
    {
        std::unique_lock<std::shared_mutex> lock(set_mutex_);
        auto it = set_data_.find(key);
        if (it != set_data_.end()) {
            it->second.has_expiry = true;
            it->second.expiry = expiry_time;
            return true;
        }
    }
    
    return false;
}

long long Storage::ttl(const std::string& key) {
    auto now = std::chrono::steady_clock::now();
    
    // Check each data type
    {
        std::shared_lock<std::shared_mutex> lock(string_mutex_);
        auto it = string_data_.find(key);
        if (it != string_data_.end()) {
            if (!it->second.has_expiry) {
                return -1; // No expiry set
            }
            if (is_expired(it->second.expiry)) {
                string_mutex_.unlock();
                std::unique_lock<std::shared_mutex> unique_lock(string_mutex_);
                string_data_.erase(it);
                return -2; // Key expired
            }
            auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
                it->second.expiry - now).count();
            return remaining;
        }
    }
    
    {
        std::shared_lock<std::shared_mutex> lock(hash_mutex_);
        auto it = hash_data_.find(key);
        if (it != hash_data_.end()) {
            if (!it->second.has_expiry) {
                return -1; // No expiry set
            }
            if (is_expired(it->second.expiry)) {
                hash_mutex_.unlock();
                std::unique_lock<std::shared_mutex> unique_lock(hash_mutex_);
                hash_data_.erase(it);
                return -2; // Key expired
            }
            auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
                it->second.expiry - now).count();
            return remaining;
        }
    }
    
    {
        std::shared_lock<std::shared_mutex> lock(list_mutex_);
        auto it = list_data_.find(key);
        if (it != list_data_.end()) {
            if (!it->second.has_expiry) {
                return -1; // No expiry set
            }
            if (is_expired(it->second.expiry)) {
                list_mutex_.unlock();
                std::unique_lock<std::shared_mutex> unique_lock(list_mutex_);
                list_data_.erase(it);
                return -2; // Key expired
            }
            auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
                it->second.expiry - now).count();
            return remaining;
        }
    }
    
    {
        std::shared_lock<std::shared_mutex> lock(set_mutex_);
        auto it = set_data_.find(key);
        if (it != set_data_.end()) {
            if (!it->second.has_expiry) {
                return -1; // No expiry set
            }
            if (is_expired(it->second.expiry)) {
                set_mutex_.unlock();
                std::unique_lock<std::shared_mutex> unique_lock(set_mutex_);
                set_data_.erase(it);
                return -2; // Key expired
            }
            auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
                it->second.expiry - now).count();
            return remaining;
        }
    }
    
    return -2; // Key doesn't exist
}