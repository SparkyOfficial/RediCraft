/*
 * config.h
 * author: Андрій Будильников
 */

#ifndef REDICRAFT_CONFIG_H
#define REDICRAFT_CONFIG_H

#include <string>
#include <unordered_map>
#include <vector>

struct ClusterNodeConfig {
    std::string host;
    int port;
    bool is_master;
    
    ClusterNodeConfig(const std::string& h, int p, bool master = false)
        : host(h), port(p), is_master(master) {}
};

class Config {
public:
    Config();
    
    // Load configuration from file
    bool load(const std::string& filename);
    
    // Get configuration values
    int getPort() const;
    std::string getHost() const;
    bool isPersistenceEnabled() const;
    std::string getPersistenceFile() const;
    int getPersistenceInterval() const; // in seconds
    
    // Replication configuration
    bool isReplicationEnabled() const;
    std::string getReplicationRole() const;
    int getReplicationPort() const;
    std::string getMasterHost() const;
    int getMasterPort() const;
    
    // Clustering configuration
    bool isClusteringEnabled() const;
    int getClusterPort() const;
    std::vector<ClusterNodeConfig> getClusterNodes() const;
    
    // Set configuration values
    void setPort(int port);
    void setHost(const std::string& host);
    void setPersistenceEnabled(bool enabled);
    void setPersistenceFile(const std::string& filename);
    void setPersistenceInterval(int interval);
    
    // Set replication configuration
    void setReplicationEnabled(bool enabled);
    void setReplicationRole(const std::string& role);
    void setReplicationPort(int port);
    void setMasterHost(const std::string& host);
    void setMasterPort(int port);
    
    // Set clustering configuration
    void setClusteringEnabled(bool enabled);
    void setClusterPort(int port);
    void addClusterNode(const std::string& host, int port, bool is_master = false);

private:
    int port_;
    std::string host_;
    bool persistence_enabled_;
    std::string persistence_file_;
    int persistence_interval_;
    
    // Replication settings
    bool replication_enabled_;
    std::string replication_role_;
    int replication_port_;
    std::string master_host_;
    int master_port_;
    
    // Clustering settings
    bool clustering_enabled_;
    int cluster_port_;
    std::vector<ClusterNodeConfig> cluster_nodes_;
    
    std::unordered_map<std::string, std::string> config_values_;
};

#endif // REDICRAFT_CONFIG_H