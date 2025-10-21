/*
 * cluster.h
 * author: Андрій Будильников
 */

#ifndef REDICRAFT_CLUSTER_H
#define REDICRAFT_CLUSTER_H

#include "storage.h"
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <shared_mutex>

// Include ASIO headers properly
#ifdef ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/net.hpp>
#else
#include <asio.hpp>
#endif

// Forward declarations for ASIO types
namespace asio {
    class io_context;
    namespace ip {
        class tcp;
    }
}

struct ClusterNode {
    std::string host;
    int port;
    bool is_master;
    mutable std::atomic<bool> is_alive;
    
    ClusterNode(const std::string& h, int p, bool master = false)
        : host(h), port(p), is_master(master), is_alive(true) {}
    
    // Add copy constructor and assignment operator to handle atomic
    ClusterNode(const ClusterNode& other)
        : host(other.host), port(other.port), is_master(other.is_master), is_alive(other.is_alive.load()) {}
    
    ClusterNode& operator=(const ClusterNode& other) {
        if (this != &other) {
            host = other.host;
            port = other.port;
            is_master = other.is_master;
            is_alive.store(other.is_alive.load());
        }
        return *this;
    }
    
    // Add move constructor and assignment operator
    ClusterNode(ClusterNode&& other) noexcept
        : host(std::move(other.host)), port(other.port), is_master(other.is_master), is_alive(other.is_alive.load()) {}
        
    ClusterNode& operator=(ClusterNode&& other) noexcept {
        if (this != &other) {
            host = std::move(other.host);
            port = other.port;
            is_master = other.is_master;
            is_alive.store(other.is_alive.load());
        }
        return *this;
    }
};

// Forward declaration for tcp::socket
namespace asio {
    namespace ip {
        class tcp;
    }
}

class ClusterManager {
public:
    ClusterManager(Storage& storage);
    ~ClusterManager();
    
    // Cluster management
    void addNode(const std::string& host, int port, bool is_master = false);
    void removeNode(const std::string& host, int port);
    void startCluster(int port);
    void stopCluster();
    
    // Node discovery
    void startNodeDiscovery();
    void stopNodeDiscovery();
    
    // Request routing
    bool routeRequest(const std::string& key, const std::string& command);
    
    // Cluster status
    std::vector<ClusterNode> getClusterNodes() const;
    bool isClusterHealthy() const;
    
private:
    Storage& storage_;
    
    // Cluster nodes
    std::vector<ClusterNode> nodes_;
    mutable std::shared_mutex nodes_mutex_;
    
    // Cluster server
    std::unique_ptr<asio::io_context> cluster_io_context_;
    std::unique_ptr<asio::ip::tcp::acceptor> cluster_acceptor_;
    std::vector<std::thread> cluster_threads_;
    std::atomic<bool> cluster_running_;
    
    // Node discovery
    std::thread discovery_thread_;
    std::atomic<bool> discovery_running_;
    
    // Helper methods
    void clusterAcceptLoop();
    void handleNodeConnection(asio::ip::tcp::socket socket);
    void nodeDiscoveryLoop();
    void pingNodes();
    
    // Hash slot calculation for sharding
    size_t calculateHashSlot(const std::string& key) const;
    
    // Find node for a specific hash slot
    ClusterNode* findNodeForSlot(size_t slot);
};

#endif // REDICRAFT_CLUSTER_H