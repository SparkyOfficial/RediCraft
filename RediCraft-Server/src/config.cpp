/*
 * config.cpp
 * author: Андрій Будильников
 */

#include "config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

Config::Config() 
    : port_(7379)
    , host_("localhost")
    , persistence_enabled_(false)
    , persistence_file_("redicraft.rdb")
    , persistence_interval_(60)
    , replication_enabled_(false)
    , replication_role_("master")
    , replication_port_(7380)
    , master_host_("localhost")
    , master_port_(7379)
    , clustering_enabled_(false)
    , cluster_port_(7381) {
}

bool Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Find the equals sign
        size_t equals_pos = line.find('=');
        if (equals_pos == std::string::npos) {
            continue;
        }
        
        // Extract key and value
        std::string key = line.substr(0, equals_pos);
        std::string value = line.substr(equals_pos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Store in our map
        config_values_[key] = value;
        
        // Update specific config values
        if (key == "port") {
            try {
                port_ = std::stoi(value);
            } catch (const std::exception&) {
                // Keep default value
            }
        } else if (key == "host") {
            host_ = value;
        } else if (key == "persistence_enabled") {
            persistence_enabled_ = (value == "true" || value == "1");
        } else if (key == "persistence_file") {
            persistence_file_ = value;
        } else if (key == "persistence_interval") {
            try {
                persistence_interval_ = std::stoi(value);
            } catch (const std::exception&) {
                // Keep default value
            }
        } else if (key == "replication_enabled") {
            replication_enabled_ = (value == "true" || value == "1");
        } else if (key == "replication_role") {
            replication_role_ = value;
        } else if (key == "replication_port") {
            try {
                replication_port_ = std::stoi(value);
            } catch (const std::exception&) {
                // Keep default value
            }
        } else if (key == "master_host") {
            master_host_ = value;
        } else if (key == "master_port") {
            try {
                master_port_ = std::stoi(value);
            } catch (const std::exception&) {
                // Keep default value
            }
        } else if (key == "clustering_enabled") {
            clustering_enabled_ = (value == "true" || value == "1");
        } else if (key == "cluster_port") {
            try {
                cluster_port_ = std::stoi(value);
            } catch (const std::exception&) {
                // Keep default value
            }
        } else if (key.substr(0, 11) == "cluster_node") {
            // Parse cluster node configuration
            // Format: cluster_node1=host:port:is_master
            size_t first_colon = value.find(':');
            size_t second_colon = value.find(':', first_colon + 1);
            
            if (first_colon != std::string::npos && second_colon != std::string::npos) {
                std::string node_host = value.substr(0, first_colon);
                std::string port_str = value.substr(first_colon + 1, second_colon - first_colon - 1);
                std::string master_str = value.substr(second_colon + 1);
                
                try {
                    int node_port = std::stoi(port_str);
                    bool is_master = (master_str == "true" || master_str == "1");
                    cluster_nodes_.emplace_back(node_host, node_port, is_master);
                } catch (const std::exception&) {
                    // Skip invalid node configuration
                }
            }
        }
    }
    
    return true;
}

int Config::getPort() const {
    return port_;
}

std::string Config::getHost() const {
    return host_;
}

bool Config::isPersistenceEnabled() const {
    return persistence_enabled_;
}

std::string Config::getPersistenceFile() const {
    return persistence_file_;
}

int Config::getPersistenceInterval() const {
    return persistence_interval_;
}

bool Config::isReplicationEnabled() const {
    return replication_enabled_;
}

std::string Config::getReplicationRole() const {
    return replication_role_;
}

int Config::getReplicationPort() const {
    return replication_port_;
}

std::string Config::getMasterHost() const {
    return master_host_;
}

int Config::getMasterPort() const {
    return master_port_;
}

bool Config::isClusteringEnabled() const {
    return clustering_enabled_;
}

int Config::getClusterPort() const {
    return cluster_port_;
}

std::vector<ClusterNodeConfig> Config::getClusterNodes() const {
    return cluster_nodes_;
}

void Config::setPort(int port) {
    port_ = port;
}

void Config::setHost(const std::string& host) {
    host_ = host;
}

void Config::setPersistenceEnabled(bool enabled) {
    persistence_enabled_ = enabled;
}

void Config::setPersistenceFile(const std::string& filename) {
    persistence_file_ = filename;
}

void Config::setPersistenceInterval(int interval) {
    persistence_interval_ = interval;
}

void Config::setReplicationEnabled(bool enabled) {
    replication_enabled_ = enabled;
}

void Config::setReplicationRole(const std::string& role) {
    replication_role_ = role;
}

void Config::setReplicationPort(int port) {
    replication_port_ = port;
}

void Config::setMasterHost(const std::string& host) {
    master_host_ = host;
}

void Config::setMasterPort(int port) {
    master_port_ = port;
}

void Config::setClusteringEnabled(bool enabled) {
    clustering_enabled_ = enabled;
}

void Config::setClusterPort(int port) {
    cluster_port_ = port;
}

void Config::addClusterNode(const std::string& host, int port, bool is_master) {
    cluster_nodes_.emplace_back(host, port, is_master);
}